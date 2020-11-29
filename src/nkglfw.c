#include "nkglfw.h"
#include "window.h"

typedef struct key_map_t {
    enum nk_keys nk_key;
    enum KEYS key;
    int control;
} key_map_t;

typedef struct mouse_map_t {
    enum nk_buttons nk_btn;
    enum BUTTONS btn;
} mouse_map_t;

static const key_map_t key_map[] = {
    {NK_KEY_DEL,             KEY_DELETE,     0},
    {NK_KEY_ENTER,           KEY_ENTER,      0},
    {NK_KEY_TAB,             KEY_TAB,        0},
    {NK_KEY_BACKSPACE,       KEY_BACKSPACE,  0},
    {NK_KEY_UP,              KEY_UP,         0},
    {NK_KEY_DOWN,            KEY_DOWN,       0},
    {NK_KEY_TEXT_START,      KEY_HOME,       0},
    {NK_KEY_TEXT_END,        KEY_END,        0},
    {NK_KEY_SCROLL_START,    KEY_HOME,       0},
    {NK_KEY_SCROLL_END,      KEY_END,        0},
    {NK_KEY_SCROLL_DOWN,     KEY_PAGE_DOWN,  0},
    {NK_KEY_SCROLL_UP,       KEY_PAGE_UP,    0},
    {NK_KEY_SHIFT,           KEY_LEFT_SHIFT, 0},
    {NK_KEY_COPY,            KEY_C,          1},
    {NK_KEY_PASTE,           KEY_V,          1},
    {NK_KEY_CUT,             KEY_X,          1},
    {NK_KEY_TEXT_UNDO,       KEY_Z,          1},
    {NK_KEY_TEXT_REDO,       KEY_R,          1},
    {NK_KEY_TEXT_WORD_LEFT,  KEY_LEFT,       1},
    {NK_KEY_TEXT_WORD_RIGHT, KEY_RIGHT,      1},
    {NK_KEY_TEXT_LINE_START, KEY_B,          1},
    {NK_KEY_TEXT_LINE_END,   KEY_E,          1},
    {NK_KEY_LEFT,            KEY_LEFT,       2},
    {NK_KEY_RIGHT,           KEY_RIGHT,      2},
};

static const mouse_map_t mouse_map[] = {
    {NK_BUTTON_LEFT, MOUSE_BUTTON_LEFT},
    {NK_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE},
    {NK_BUTTON_RIGHT, MOUSE_BUTTON_RIGHT},
};

static const size_t key_map_size = sizeof(key_map) / sizeof(key_map_t);

static const size_t mouse_map_size =
        sizeof(mouse_map) / sizeof(mouse_map_t);

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
    mesh_subinit(&nkglfw->mesh, "nuklear");
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

void nk_glfw_new_frame(NkGlfw* nkglfw) {
    int i, *cursor;
    struct nk_context *ctx = &nkglfw->context;
    struct Window *win = nkglfw->window;
    const bool control_pressed =
            window_is_key_pressed(win, KEY_LEFT_CONTROL) ||
            window_is_key_pressed(win, KEY_RIGHT_CONTROL);
    bool is_pressed;

    nk_input_begin(ctx);
    for (i = 0; i < nkglfw->text_len && i < 64; ++i)
        nk_input_unicode(ctx, nkglfw->text[i]);

    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
        window_grab_mouse(nkglfw->window, true);
    else if (ctx->input.mouse.ungrab)
        window_grab_mouse(nkglfw->window, false);

    for (size_t i = 0; i < key_map_size; i++) {
        if (!key_map[i].control ||
                (bool)(key_map[i].control - 1) == !control_pressed) {
            is_pressed = window_is_key_pressed(win, key_map[i].key);
            nk_input_key(ctx, key_map[i].nk_key, is_pressed);
        }
    }

    if (control_pressed) {
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
        nk_input_key(ctx, NK_KEY_SHIFT, 0);
    }

    cursor = window_get_cursor(win);
    nk_input_motion(ctx, cursor[0], cursor[1]);
    if (ctx->input.mouse.grabbed) {
        window_set_cursor(
                    win, (int[2]){(int)ctx->input.mouse.prev.x,
                    (int)ctx->input.mouse.prev.y});
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }

    for (size_t i = 0; i < mouse_map_size; i++) {
        is_pressed = window_is_mouse_pressed(win, mouse_map[i].btn);
        nk_input_button(ctx, mouse_map[i].nk_btn,
                        cursor[0], cursor[1], is_pressed);
    }
    cursor = window_get_double_click_pos(win);
    nk_input_button(
                ctx, NK_BUTTON_DOUBLE, cursor[0], cursor[1],
                window_is_double_clicked(win));
    nk_input_scroll(ctx, nkglfw->scroll);
    nk_input_end(&nkglfw->context);
    nkglfw->text_len = 0;
    nkglfw->scroll = nk_vec2(0, 0);
}

void nk_glfw_render(NkGlfw* nkglfw) {
    int *size = window_get_size(nkglfw->window),
            *display = window_get_display(nkglfw->window);
    float *scale = window_get_scale(nkglfw->window);
    nkglfw->ortho[0][0] = 2.0f/(GLfloat)size[0];
    nkglfw->ortho[1][1] = -2.0f/(GLfloat)size[1];

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

void nk_glfw_char_callback(NkGlfw* nkglfw, unsigned int codepoint) {
    if (nkglfw->text_len < NK_GLFW_TEXT_MAX)
        nkglfw->text[nkglfw->text_len++] = codepoint;
}

void nk_gflw_scroll_callback(NkGlfw* nkglfw, double xoff, double yoff) {
    nkglfw->scroll.x += (float)xoff;
    nkglfw->scroll.y += (float)yoff;
}
