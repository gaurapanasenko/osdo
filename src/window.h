#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>

#include "scene.h"
#include "mesh.h"
#include "uthash.h"
#include "camera.h"

typedef struct App App;

typedef struct Window {
    // screen size
    int screen_width, screen_height;
    bool mouse_capute;
    float last_x, last_y;

    GlMesh *meshes;
    Scene *scene;
    GLFWwindow *window;
    Camera camera;

    // buffered data for loop
    mat4 mat4buf, projection;
    vec4 vec4buf;
    GLfloat resolution;
} Window;

int window_init(App *app, Window *win, Scene* scene);

void window_del(Window *win);

int window_loop(App *app, Window *win);

// process all input
// -----------------
void window_process_input(App *app, Window *win);

#endif // WINDOW_H
