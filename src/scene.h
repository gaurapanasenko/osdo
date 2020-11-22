#ifndef SCENE_H
#define SCENE_H
#include <GLFW/glfw3.h>

#include "conf.h"
#include "basis.h"

typedef struct Renderer {
    // screen size
    int screen_width;
    int screen_height;

    // camera
    Basis basis[3];
    float lastX;
    float lastY;
    bool firstMouse;

    // timing
    GLdouble deltaTime; // time between current frame and last frame
    GLdouble lastFrame;

    // active element
    int active;

    // states
    bool waterframe;
    bool light;
    bool dirLightOn;
    bool pointLight1On;
    bool pointLight2On;
    bool spotLightOn;

    GLfloat zoom;
} Renderer;

void scene_init(void);
void scene_reset();

// process all input
// -----------------
void scene_process_input(GLFWwindow *window);

// glfw: when the window size changed this callback function executes
// ------------------------------------------------------------------
void scene_resize(GLFWwindow* window, GLint width,
                  GLint height);

// glfw: when the mouse scroll wheel scrolls, this callback is called
// ------------------------------------------------------------------
void scene_scroll(GLFWwindow* window, GLdouble xoffset,
                  GLdouble yoffset);

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void scene_key(GLFWwindow* window, int key, int scancode,
               int action, int mods);

int scene_loop(GLFWwindow *window);

#endif // SCENE_H
