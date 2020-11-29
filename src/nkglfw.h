#ifndef NKGLFW_H
#define NKGLFW_H

#include "osdo.h"
#include "conf.h"

#include "shader.h"
#include "nuklear.h"
#include "window.h"

#define NK_GLFW_TEXT_MAX 256

typedef struct NkGlfw {
    Shader *shader;
    Mesh mesh;
    struct nk_context context;
    struct nk_font_atlas atlas;
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    struct nk_vec2 scroll;
    struct nk_convert_config config;
    unsigned int text[NK_GLFW_TEXT_MAX];
    int text_len;
    GLuint font_tex;
    Window *window;
    mat4 ortho;
} NkGlfw;

void nk_glfw_init(NkGlfw* nkglfw, Window *window, Shader *shader);
void nk_glfw_del(NkGlfw* nkglfw);

void nk_glfw_font_stash_begin(NkGlfw* nkglfw,
                              struct nk_font_atlas **atlas);
void nk_glfw_font_stash_end(NkGlfw* nkglfw);

void nk_glfw_new_frame(NkGlfw* nkglfw);
void nk_glfw_render(NkGlfw* nkglfw);

void nk_glfw_char_callback(NkGlfw* nkglfw, unsigned int codepoint);
void nk_gflw_scroll_callback(NkGlfw* nkglfw, double xoff, double yoff);
void nk_glfw_mouse_button_callback(
        NkGlfw* nkglfw, int button, int action, int mods);

#endif // NKGLFW_H
