#ifndef WINDOW_H
#define WINDOW_H

#include "osdo.h"

struct GLFWwindow;

enum KEYS {
    KEY_DELETE,
    KEY_ENTER,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_LEFT_SHIFT,
    KEY_RIGHT_SHIFT,
    KEY_LEFT_CONTROL,
    KEY_RIGHT_CONTROL,
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
    KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
};

enum BUTTONS {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
};

struct Window;

typedef void (*scroll_cb_t)(
        struct Window *window, GLdouble xoffset, GLdouble yoffset);
typedef void (*mouse_motion_cb_t)(
        struct Window *window, int offset[2]);
typedef void (*char_cb_t)(
        struct Window *window, unsigned int codepoint);
typedef void (*mouse_button_cb_t)(
        struct Window *window, enum BUTTONS button, bool pressed);
typedef void (*key_cb_t)(
        struct Window *window, enum KEYS key, bool pressed);

typedef struct Window {
    struct GLFWwindow *window;
    void *user_pointer;

    // screen size
    int size[2], display[2], double_click_pos[2];
    vec2 scale;
    int cursor[2];
    bool mouse_capute;
    bool double_click;
    double last_click_time, current_time, last_time, delta_time;

    scroll_cb_t scroll_cb;
    mouse_motion_cb_t mouse_motion_cb;
    char_cb_t char_cb;
    mouse_button_cb_t mouse_button_cb;
    key_cb_t key_cb;
} Window;

int window_init(Window *window);
void window_del(Window *window);
bool window_pre_loop(Window *window);
void window_post_loop(Window *window);

void window_set_user_pointer(Window *window, void *pointer);
void *window_get_user_pointer(Window *window);

float window_get_resolution(Window *window);

double window_get_delta_time(Window *window);

const char * window_get_clipboard(Window *window);
void window_set_clipboard(Window *window, const char * str);

bool window_is_mouse_caputed(Window *window);
void window_set_mouse_capute(Window *window, bool capute);
void window_grab_mouse(Window *window, bool grab);

bool window_is_key_pressed(Window *window, enum KEYS key);
bool window_is_mouse_pressed(Window *window, enum BUTTONS key);

int *window_get_cursor(Window *window);
void window_update_cursor(Window *window);
void window_set_cursor(Window *window, int coords[2]);

int *window_get_size(Window *window);
int *window_get_display(Window *window);
float *window_get_scale(Window *window);
int *window_get_double_click_pos(Window *window);
bool window_is_double_clicked(Window *window);

void window_set_scroll_cb(Window *window, scroll_cb_t callback);
void window_set_mouse_motion_cb(
        Window *window, mouse_motion_cb_t callback);
void window_set_char_cb(Window *window, char_cb_t callback);
void window_set_mouse_button_cb(
        Window *window, mouse_button_cb_t callback);
void window_set_key_cb(Window *window, key_cb_t callback);

void window_resize_cb(
        struct GLFWwindow* window, GLint width, GLint height);
void window_scroll_cb(
        struct GLFWwindow* window, GLdouble xoffset, GLdouble yoffset);
void window_mouse_motion_cb(
        struct GLFWwindow* window, double xpos, double ypos);
void window_char_cb(struct GLFWwindow* window, unsigned int codepoint);
void window_mouse_button_cb(
        struct GLFWwindow *window, int button, int action, int mods);
void window_key_cb(struct GLFWwindow* window, int key, int scancode,
                   int action, int mods);

#endif // WINDOW_H
