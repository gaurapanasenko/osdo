#ifndef APP_H
#define APP_H
#include "window.h"
#include "conf.h"

typedef struct App {
    Mesh meshes[MESHES_NUM];
    GLuint lighting_shader, lightless_shader;
    GLFWwindow *main_window;
    Window windows[WINDOWS_NUM];
    Scene scene;

    // timing
    GLdouble delta_time;
} App;

int app_init(void);

int app_loop(void);

bool app_should_closed(void);

Window *app_search_window(GLFWwindow *window);

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void app_resize(GLFWwindow* window, GLint width,
                  GLint height);

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void app_scroll(GLFWwindow* window, GLdouble xoffset,
                  GLdouble yoffset);

void app_mouse(GLFWwindow* window, double xpos, double ypos);

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void app_key(GLFWwindow* window, int key, int scancode,
               int action, int mods);

#endif // APP_H
