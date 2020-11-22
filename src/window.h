#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>

#include "scene.h"
#include "mesh.h"

typedef struct App App;

typedef struct Window {
    // screen size
    int screen_width;
    int screen_height;

    GlMesh e, p, cube;
    Scene scene;
    GLFWwindow *window;

    // buffered data for loop
    mat4 mat4buf, *camera_view, camera_rotation, projection;
    GLfloat resolution;
    vec4 camera_position, camera_look_direction;
} Window;

int window_init(App *app, Window *win);

void window_del(Window *win);

int window_loop(App *app, Window *win);

// process all input
// -----------------
void window_process_input(App *app, Window *win);

#endif // WINDOW_H
