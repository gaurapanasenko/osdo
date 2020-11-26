#ifndef WINDOW_H
#define WINDOW_H
#include <GLFW/glfw3.h>
#include <nuklear.h>

#include "scene.h"
#include "mesh.h"
#include "uthash.h"
#include "camera.h"

struct App;

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

int window_init(struct App *app, Window *win, Scene* scene);

void window_del(struct Window *win);

int window_loop(struct App *app, struct Window *win);

// process all input
// -----------------
void window_process_input(struct App *app, struct Window *win);

#endif // WINDOW_H
