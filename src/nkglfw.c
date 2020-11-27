#include <cglm/cglm.h>
#include "GL/glew.h"
#include "GL/gl.h"
#include "nkglfw.h"
#include "app.h"

void nk_glfw_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    App* app = usr.ptr;
    const char *text = glfwGetClipboardString(app->window);
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
    App* app = usr.ptr;
    glfwSetClipboardString(app->window, str);
    free(str);
}

void nk_glfw_init(NkGlfw* nkglfw, App *app,
                  Mesh *mesh, Shader *shader) {
    nk_init_default(&nkglfw->context, 0);
    nkglfw->context.clip.copy = nk_glfw_clipboard_copy;
    nkglfw->context.clip.paste = nk_glfw_clipboard_paste;
    nkglfw->context.clip.userdata = nk_handle_ptr(app);
    nkglfw->last_button_click = 0;

    nkglfw->is_double_click_down = nk_false;
    nkglfw->double_click_pos = nk_vec2(0, 0);

    nk_buffer_init_default(&nkglfw->cmds);
    nkglfw->mesh = mesh;
    nkglfw->shader = shader;
    struct nk_font_atlas *atlas;
    nk_glfw_font_stash_begin(nkglfw, &atlas);
    nk_glfw_font_stash_end(nkglfw);
}

void nk_glfw_del(NkGlfw* nkglfw) {
    glDeleteTextures(1, &nkglfw->font_tex);
    nk_buffer_free(&nkglfw->cmds);
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

void nk_glfw_new_frame(NkGlfw* nkglfw, App *app) {
    int i;
    double x, y;
    struct nk_context *ctx = &nkglfw->context;
    struct GLFWwindow *win = app->window;

    nk_input_begin(ctx);
    for (i = 0; i < nkglfw->text_len; ++i)
        nk_input_unicode(ctx, nkglfw->text[i]);

    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
        glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    else if (ctx->input.mouse.ungrab)
        glfwSetInputMode(app->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    nk_input_key(ctx, NK_KEY_DEL,
                 glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_ENTER,
                 glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TAB,
                 glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_BACKSPACE,
                 glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_UP,
                 glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_DOWN,
                 glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_START,
                 glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_END,
                 glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_START,
                 glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_END,
                 glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN,
                 glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SCROLL_UP,
                 glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SHIFT,
                 glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                 glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        nk_input_key(ctx, NK_KEY_COPY,
                     glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_PASTE,
                     glfwGetKey(win, GLFW_KEY_V) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_CUT,
                     glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_UNDO,
                     glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_REDO,
                     glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT,
                     glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT,
                     glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START,
                     glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END,
                     glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
    } else {
        nk_input_key(ctx, NK_KEY_LEFT,
                     glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_RIGHT,
                     glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
        nk_input_key(ctx, NK_KEY_SHIFT, 0);
    }

    glfwGetCursorPos(win, &x, &y);
    nk_input_motion(ctx, (int)x, (int)y);
    if (ctx->input.mouse.grabbed) {
        glfwSetCursorPos(app->window, (GLdouble)ctx->input.mouse.prev.x,
                         (GLdouble)ctx->input.mouse.prev.y);
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }
    nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y,
                    glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y,
                    glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y,
                    glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_DOUBLE, (int)nkglfw->double_click_pos.x,
            (int)nkglfw->double_click_pos.y, nkglfw->is_double_click_down);
    nk_input_scroll(ctx, nkglfw->scroll);
    nk_input_end(&nkglfw->context);
    nkglfw->text_len = 0;
    nkglfw->scroll = nk_vec2(0, 0);
}

void nk_glfw_render(NkGlfw* nkglfw, App *app,
                    enum nk_anti_aliasing AA, int max_vertex_buffer,
                    int max_element_buffer) {
    struct nk_buffer vbuf, ebuf;
    Shader *sh = nkglfw->shader;
    Mesh *mesh = nkglfw->mesh;
    const GLint width = app->width, height = app->height;
    mat4 ortho = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

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
    shader_set_int(sh, "frag_texture", 0);
    shader_set_mat4(sh, "projection", ortho);
    glViewport(0, 0, (GLsizei)app->display_width, (GLsizei)app->display_height);
    {
        /* convert from command queue into draw list
         * and draw to screen */
        const struct nk_draw_command *cmd;
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        /* allocate vertex and element buffer */
        glBindVertexArray(mesh->vao);

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL,
                     GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL,
                     GL_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex
         * + element buffer */
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        {
            /* fill convert configuration */
            struct nk_convert_config config;
            static const struct nk_draw_vertex_layout_element vertex_layout[] = {
            {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, position)},
            {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(Vertex, color)},
            {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, uv)},
            {NK_VERTEX_LAYOUT_END}};
            memset(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(Vertex);
            config.vertex_alignment = NK_ALIGNOF(Vertex);
            config.null = nkglfw->null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            nk_buffer_init_fixed(
                        &vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(
                        &ebuf, elements, (size_t)max_element_buffer);
            nk_convert(&nkglfw->context, &nkglfw->cmds,
                       &vbuf, &ebuf, &config);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &nkglfw->context, &nkglfw->cmds)
        {
            if (!cmd->elem_count) continue;
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
            glScissor(
                (GLint)(cmd->clip_rect.x * app->framebuffer_scale[0]),
                (GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h))
                        * (GLint)app->framebuffer_scale[1]),
                (GLint)(cmd->clip_rect.w * app->framebuffer_scale[0]),
                (GLint)(cmd->clip_rect.h * app->framebuffer_scale[1]));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count,
                           GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&nkglfw->context);
        nk_buffer_clear(&nkglfw->cmds);
    }

    /* default OpenGL state */
    glUseProgram(0);
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

void nk_glfw_mouse_button_callback(
        NkGlfw* nkglfw, App *app, int button, int action,
        UNUSED int mods) {
    double x, y;
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    glfwGetCursorPos(app->window, &x, &y);
    if (action == GLFW_PRESS)  {
        double dt = glfwGetTime() - nkglfw->last_button_click;
        if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {
            nkglfw->is_double_click_down = nk_true;
            nkglfw->double_click_pos = nk_vec2((float)x, (float)y);
        }
        nkglfw->last_button_click = glfwGetTime();
    } else nkglfw->is_double_click_down = nk_false;
}
