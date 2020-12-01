#include "nkglfw.h"
#include "window.h"

static const struct nk_draw_vertex_layout_element vertex_layout[] = {
    {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, position)},
    {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(Vertex, color)},
    {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, uv)},
    {NK_VERTEX_LAYOUT_END}
};

void nk_glfw_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    Window* window = usr.ptr;
    const char *text = window_get_clipboard(window);
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

void nk_glfw_clipboard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    Window* window = usr.ptr;
    window_set_clipboard(window, str);
    free(str);
}

void nk_glfw_init(NkGlfw* nkglfw, Window *window, Shader *shader) {
    nk_init_default(&nkglfw->context, 0);
    nkglfw->context.clip.copy = nk_glfw_clipboard_copy;
    nkglfw->context.clip.paste = nk_glfw_clipboard_paste;
    nkglfw->context.clip.userdata = nk_handle_ptr(window);

    nk_buffer_init_default(&nkglfw->cmds);
    mesh_subinit(&nkglfw->mesh);
    nkglfw->shader = shader;
    nkglfw->window = window;
    struct nk_font_atlas *atlas;
    nk_glfw_font_stash_begin(nkglfw, &atlas);
    nk_glfw_font_stash_end(nkglfw);

    /* fill convert configuration */
    struct nk_convert_config *config = &nkglfw->config;
    memset(config, 0, sizeof(*config));
    config->vertex_layout = vertex_layout;
    config->vertex_size = sizeof(Vertex);
    config->vertex_alignment = NK_ALIGNOF(Vertex);
    config->null = nkglfw->null;
    config->circle_segment_count = 22;
    config->curve_segment_count = 22;
    config->arc_segment_count = 22;
    config->global_alpha = 1.0f;
    config->shape_AA = NK_ANTI_ALIASING_ON;
    config->line_AA = NK_ANTI_ALIASING_ON;

    glm_mat4_copy((mat4){
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    }, nkglfw->ortho);
}

void nk_glfw_del(NkGlfw* nkglfw) {
    glDeleteTextures(1, &nkglfw->font_tex);
    nk_buffer_free(&nkglfw->cmds);
    mesh_del(&nkglfw->mesh);
}

void nk_glfw_font_stash_begin(NkGlfw* nkglfw,
                              struct nk_font_atlas **atlas) {
    nk_font_atlas_init_default(&nkglfw->atlas);
    nk_font_atlas_begin(&nkglfw->atlas);
    *atlas = &nkglfw->atlas;
}

void nk_glfw3_device_upload_atlas(NkGlfw* nkglfw, const void *image,
                                  int width, int height) {
    glGenTextures(1, &nkglfw->font_tex);
    glBindTexture(GL_TEXTURE_2D, nkglfw->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 (GLsizei)width, (GLsizei)height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void nk_glfw_font_stash_end(NkGlfw* nkglfw) {
    const void *image;
    int w, h;
    image = nk_font_atlas_bake(&nkglfw->atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    nk_glfw3_device_upload_atlas(nkglfw, image, w, h);
    nk_font_atlas_end(&nkglfw->atlas, nk_handle_id((int)nkglfw->font_tex),
                      &nkglfw->null);
    if (nkglfw->atlas.default_font)
        nk_style_set_font(&nkglfw->context,
                          &nkglfw->atlas.default_font->handle);
}

void nk_glfw_begin_input(NkGlfw* nkglfw) {
    nk_input_begin(&nkglfw->context);
}

void nk_glfw_end_input(NkGlfw* nkglfw) {
    nk_input_end(&nkglfw->context);
}

void nk_glfw_render(NkGlfw* nkglfw) {
    int *size = window_get_size(nkglfw->window),
            *display = window_get_display(nkglfw->window);
    float *scale = window_get_scale(nkglfw->window);
    nkglfw->ortho[0][0] = 2.0f/(GLfloat)size[0];
    nkglfw->ortho[1][1] = -2.0f/(GLfloat)size[1];

    if (nkglfw->context.input.mouse.grab)
        window_grab_mouse(nkglfw->window, true);
    if (nkglfw->context.input.mouse.ungrab)
        window_grab_mouse(nkglfw->window, false);

    /* setup global state */
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    /* setup program */
    shader_use(nkglfw->shader);
    shader_set_int(nkglfw->shader, "frag_texture", 0);
    shader_set_mat4(nkglfw->shader, "projection", nkglfw->ortho);
    glViewport(0, 0, display[0], display[1]);
    {
        /* convert from command queue into draw list
         * and draw to screen */
        const struct nk_draw_command *cmd;
        const nk_draw_index *offset = NULL;

        /* allocate vertex and element buffer */
        glBindVertexArray(nkglfw->mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, nkglfw->mesh.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nkglfw->mesh.ebo);

        glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_BUFFER, NULL,
                     GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_ELEMENT_BUFFER, NULL,
                     GL_STREAM_DRAW);
        {
            /* load draw vertices & elements directly into vertex
             * + element buffer */
            struct nk_buffer vbuf, ebuf;
            void *vertices, *elements;
            vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
            elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
            /* setup buffers to load vertices and elements */
            nk_buffer_init_fixed(
                        &vbuf, vertices, (size_t)MAX_VERTEX_BUFFER);
            nk_buffer_init_fixed(
                        &ebuf, elements, (size_t)MAX_ELEMENT_BUFFER);
            nk_convert(&nkglfw->context, &nkglfw->cmds,
                       &vbuf, &ebuf, &nkglfw->config);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &nkglfw->context, &nkglfw->cmds)
        {
            if (!cmd->elem_count) continue;
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x * scale[0]),
                ((size[1] - (GLint)(cmd->clip_rect.y +
                                    cmd->clip_rect.h)) * (GLint)scale[1]),
                (GLint)(cmd->clip_rect.w * scale[0]),
                (GLint)(cmd->clip_rect.h * scale[1]));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count,
                           GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&nkglfw->context);
        nk_buffer_clear(&nkglfw->cmds);
    }

    /* default OpenGL state */
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

void nk_gflw_scroll_callback(NkGlfw* nkglfw, double xoff, double yoff) {
    nk_input_scroll(&nkglfw->context, nk_vec2((float)xoff, (float)yoff));
}

void nk_glfw_mouse_callback(
        NkGlfw* nkglfw, vec2 pos, UNUSED vec2 offset) {
    struct nk_context *ctx = &nkglfw->context;
    nk_input_motion(ctx, (int)pos[0], (int)pos[1]);
}

void nk_glfw_char_callback(NkGlfw* nkglfw, unsigned int codepoint) {
    nk_input_unicode(&nkglfw->context, codepoint);
}

void nk_glfw_mouse_button_callback(
        NkGlfw* nkglfw, enum BUTTONS button, bool pressed) {
    vec2 cursor;
    window_get_cursor(nkglfw->window, cursor);
    int c[2] = {(int)cursor[0], (int)cursor[1]};
    struct nk_context *ctx = &nkglfw->context;
    switch (button) {
    case MOUSE_BUTTON_LEFT:
        nk_input_button(ctx, NK_BUTTON_LEFT, c[0], c[1],
                pressed);
        break;
    case MOUSE_BUTTON_MIDDLE:
        nk_input_button(ctx, NK_BUTTON_MIDDLE, c[0], c[1],
                pressed);
        break;
    case MOUSE_BUTTON_RIGHT:
        nk_input_button(ctx, NK_BUTTON_RIGHT, c[0], c[1],
                pressed);
        break;
    case MOUSE_BUTTON_DOUBLE:
        nk_input_button(ctx, NK_BUTTON_DOUBLE, c[0], c[1],
                pressed);
        break;
    }
}

void nk_glfw_key_callback(
        NkGlfw* nkglfw, enum KEYS key, bool pressed) {
    struct nk_context *ctx = &nkglfw->context;
    enum nk_keys k[16], inv[16];
    Window *win = nkglfw->window;
    int n = 0, m = 0;
    const bool ctrl =
            window_is_key_pressed(win, KEY_LEFT_CONTROL) ||
            window_is_key_pressed(win, KEY_RIGHT_CONTROL);
    switch (key) {
    case KEY_DELETE:      k[n++] = NK_KEY_DEL;             break;
    case KEY_ENTER:       k[n++] = NK_KEY_ENTER;           break;
    case KEY_TAB:         k[n++] = NK_KEY_TAB;             break;
    case KEY_BACKSPACE:   k[n++] = NK_KEY_BACKSPACE;       break;
    case KEY_UP:          k[n++] = NK_KEY_UP;              break;
    case KEY_DOWN:        k[n++] = NK_KEY_DOWN;            break;
    case KEY_PAGE_DOWN:   k[n++] = NK_KEY_SCROLL_DOWN;     break;
    case KEY_PAGE_UP:     k[n++] = NK_KEY_SCROLL_UP;       break;
    case KEY_C:           k[n++] = NK_KEY_COPY;            break;
    case KEY_V: if (ctrl) k[n++] = NK_KEY_PASTE;           break;
    case KEY_X: if (ctrl) k[n++] = NK_KEY_CUT;             break;
    case KEY_Z: if (ctrl) k[n++] = NK_KEY_TEXT_UNDO;       break;
    case KEY_R: if (ctrl) k[n++] = NK_KEY_TEXT_REDO;       break;
    case KEY_B: if (ctrl) k[n++] = NK_KEY_TEXT_LINE_START; break;
    case KEY_E: if (ctrl) k[n++] = NK_KEY_TEXT_LINE_END;   break;
    case KEY_LEFT:
        if (ctrl) k[n++] = NK_KEY_TEXT_WORD_LEFT;
        else k[n++] = NK_KEY_LEFT;
        break;
    case KEY_RIGHT:
        if (ctrl) k[n++] = NK_KEY_TEXT_WORD_RIGHT;
        else k[n++] = NK_KEY_RIGHT;
        break;
    case KEY_HOME:
        k[n++] = NK_KEY_TEXT_START; k[n++] = NK_KEY_SCROLL_START;
        break;
    case KEY_END:
        k[n++] = NK_KEY_TEXT_END;   k[n++] = NK_KEY_SCROLL_END;
        break;
    case KEY_LEFT_CONTROL:
    case KEY_RIGHT_CONTROL:
        if (window_is_key_pressed(win, KEY_V))
            k[n++] = NK_KEY_PASTE;
        if (window_is_key_pressed(win, KEY_X))
            k[n++] = NK_KEY_CUT;
        if (window_is_key_pressed(win, KEY_Z))
            k[n++] = NK_KEY_TEXT_UNDO;
        if (window_is_key_pressed(win, KEY_R))
            k[n++] = NK_KEY_TEXT_REDO;
        if (window_is_key_pressed(win, KEY_B))
            k[n++] = NK_KEY_TEXT_LINE_START;
        if (window_is_key_pressed(win, KEY_E))
            k[n++] = NK_KEY_TEXT_LINE_END;
        if (window_is_key_pressed(win, KEY_LEFT)) {
            k[n++] = NK_KEY_TEXT_WORD_LEFT;
            inv[m++] = NK_KEY_LEFT;
        } else {
            inv[m++] = NK_KEY_TEXT_WORD_LEFT;
            k[n++] = NK_KEY_LEFT;
        }
        if (window_is_key_pressed(win, KEY_RIGHT)) {
            k[n++] = NK_KEY_TEXT_WORD_RIGHT;
            inv[m++] = NK_KEY_RIGHT;
        } else {
            inv[m++] = NK_KEY_TEXT_WORD_RIGHT;
            k[n++] = NK_KEY_RIGHT;
        }
        break;
    default:
        break;
    }
    for (int i = 0; i < n; i++)
        if (k[i] != NK_KEY_NONE)
            nk_input_key(ctx, k[i], pressed);
    for (int i = 0; i < m; i++)
        if (inv[i] != NK_KEY_NONE)
            nk_input_key(ctx, inv[i], !pressed);
}
