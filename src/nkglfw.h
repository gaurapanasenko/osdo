#ifndef NKGLFW_H
#define NKGLFW_H

#include "shader.h"
#include "conf.h"
#include "nuklear.h"

#define NK_GLFW_TEXT_MAX 256

struct App;

typedef struct NkGlfw {
    Shader *shader;
    Mesh *mesh;
    struct nk_context context;
    struct nk_font_atlas atlas;
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    struct nk_vec2 scroll;
    unsigned int text[NK_GLFW_TEXT_MAX];
    int text_len;
    GLuint font_tex;
    double last_button_click;
    int is_double_click_down;
    struct nk_vec2 double_click_pos;
} NkGlfw;

void nk_glfw_init(NkGlfw* nkglfw, struct App *app,
                  Mesh* mesh, Shader *shader);
void nk_glfw_del(NkGlfw* nkglfw);

void nk_glfw_font_stash_begin(NkGlfw* nkglfw,
                              struct nk_font_atlas **atlas);
void nk_glfw_font_stash_end(NkGlfw* nkglfw);

void nk_glfw_new_frame(NkGlfw* nkglfw, struct App *app);
void nk_glfw_render(NkGlfw* nkglfw, struct App* app,
                    enum nk_anti_aliasing, int max_vertex_buffer,
                    int max_element_buffer);

void nk_glfw_char_callback(NkGlfw* nkglfw, unsigned int codepoint);
void nk_gflw_scroll_callback(NkGlfw* nkglfw, double xoff, double yoff);
void nk_glfw_mouse_button_callback(NkGlfw* nkglfw, struct App* app,
                                   int button, int action, int mods);

#endif // NKGLFW_H
