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
    MOUSE_BUTTON_DOUBLE,
};

class Window;

typedef void (*scroll_cb_t)(
        Window *window, GLdouble xoffset, GLdouble yoffset);
typedef void (*mouse_motion_cb_t)(
        Window *window, vec2 pos, vec2 offset);
typedef void (*char_cb_t)(
        Window *window, unsigned int codepoint);
typedef void (*mouse_button_cb_t)(
        Window *window, enum BUTTONS button, bool pressed);
typedef void (*key_cb_t)(
        Window *window, enum KEYS key, bool pressed);

class Window {
    struct GLFWwindow *window;
    void *user_pointer;

    // screen size
    int size[2], display[2];
    vec2 scale, cursor;
    bool mouse_capute;
    double last_click_time, current_time, last_time, delta_time;

    scroll_cb_t _scroll_cb;
    mouse_motion_cb_t _mouse_motion_cb;
    char_cb_t _char_cb;
    mouse_button_cb_t _mouse_button_cb;
    key_cb_t _key_cb;
public:
    ~Window();

    Window() = default;
    Window(const Window&) = delete;
    Window(Window&&) = delete;

    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    int init();

    struct GLFWwindow* get();

    bool alive();
    bool pre_loop();
    void post_loop();

    void set_user_pointer(void *pointer);
    void *get_user_pointer();

    float get_resolution();

    double get_delta_time();

    const char * get_clipboard();
    void set_clipboard(const char * str);

    bool is_mouse_caputed();
    void grab_mouse(bool grab);

    bool is_key_pressed(enum KEYS key);
    bool is_mouse_pressed(enum BUTTONS key);

    void get_cursor(vec2 dest);
    void set_cursor(vec2 coords);

    int *get_size();
    int *get_display();
    float *get_scale();

    void set_scroll_cb(scroll_cb_t callback);
    void set_mouse_motion_cb(mouse_motion_cb_t callback);
    void set_char_cb(char_cb_t callback);
    void set_mouse_button_cb(mouse_button_cb_t callback);
    void set_key_cb(key_cb_t callback);

    static void resize_cb(
            struct GLFWwindow* window, GLint width, GLint height);
    static void scroll_cb(
            struct GLFWwindow* window, GLdouble xoffset, GLdouble yoffset);
    static void mouse_motion_cb(
            struct GLFWwindow* window, double xpos, double ypos);
    static void char_cb(struct GLFWwindow* window, unsigned int codepoint);
    static void mouse_button_cb(
            struct GLFWwindow *window, int button, int action, int mods);
    static void key_cb(struct GLFWwindow* window, int key, int scancode,
                       int action, int mods);
};

#endif // WINDOW_H
