#ifndef APP_H
#define APP_H

#include "osdo.h"
#include "conf.h"

#include "shader.h"
#include "mesh.h"
#include "scene.h"
#include "camera.h"
#include "nkglfw.h"
#include "model.h"
#include "window.h"

typedef struct App {
    Model *models;
    Shader *shaders;
    Scene scene;
    UT_array *objects;
    Camera camera;
    NkGlfw nkglfw;
    Window window;

    // buffered data for loop
    mat4 mat4buf, projection, last_camera;
    vec4 vec4buf;
} App;

int app_init(App *app);

void app_del(App *app);

int app_loop(App *app);

bool app_load_shader(App *app, const char *name);

void app_scroll(Window* window, GLdouble xoffset, GLdouble yoffset);
void app_mouse(Window* window, int offset[2]);
void app_char_callback(Window* window, unsigned int codepoint);
void app_mouse_button_callback(
        Window *window, enum BUTTONS button, bool pressed);
void app_key(Window* window, enum KEYS key, bool pressed);

// process all input
// -----------------
void app_process_input(App *app);

#endif // APP_H
