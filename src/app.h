#ifndef APP_H
#define APP_H
#include <GLFW/glfw3.h>
#include "conf.h"
#include "shader.h"
#include "mesh.h"
#include "scene.h"
#include "camera.h"
#include "utarray.h"
#include "nkglfw.h"
#include "bmesh.h"

typedef struct App {
    UT_array *bmeshes;
    Mesh *meshes;
    Shader *shaders;
    Scene scene;
    GLFWwindow *window;
    UT_array *objects;
    Camera camera;
    NkGlfw nkglfw;

    // timing
    GLdouble delta_time;

    // screen size
    GLint width, height, display_width, display_height;
    vec2 framebuffer_scale;
    bool mouse_capute;
    float last_x, last_y;

    // buffered data for loop
    mat4 mat4buf, projection, last_camera;
    vec4 vec4buf;
} App;

int app_init(void);

void app_del(void);

int app_loop(void);

bool app_load_shader(const char *name);

bool app_should_closed(void);

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void app_resize(GLFWwindow* window, GLint width,
                  GLint height);

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void app_scroll(GLFWwindow* window, GLdouble xoffset,
                  GLdouble yoffset);

void app_mouse(GLFWwindow* window, double xpos, double ypos);

void app_char_callback(GLFWwindow* window, unsigned int codepoint);
void app_mouse_button_callback(
        GLFWwindow *window, int button, int action, int mods);

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(GLFWwindow* window, int key, int scancode,
               int action, int mods);

// process all input
// -----------------
void app_process_input(void);

#endif // APP_H
