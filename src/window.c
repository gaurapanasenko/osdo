#include "window.h"
#include "conf.h"
#include <GLFW/glfw3.h>

typedef struct key_map_t {
    enum KEYS key;
    int glfw_key;
} key_map_t;

typedef struct mouse_map_t {
    enum BUTTONS btn;
    int glfw_btn;
} mouse_map_t;

static const key_map_t key_list[] = {
    {KEY_DELETE,        GLFW_KEY_DELETE},
    {KEY_ENTER,         GLFW_KEY_ENTER},
    {KEY_TAB,           GLFW_KEY_TAB},
    {KEY_BACKSPACE,     GLFW_KEY_BACKSPACE},
    {KEY_UP,            GLFW_KEY_UP},
    {KEY_DOWN,          GLFW_KEY_DOWN},
    {KEY_LEFT,          GLFW_KEY_LEFT},
    {KEY_RIGHT,         GLFW_KEY_RIGHT},
    {KEY_HOME,          GLFW_KEY_HOME},
    {KEY_END,           GLFW_KEY_END},
    {KEY_PAGE_UP,       GLFW_KEY_PAGE_UP},
    {KEY_PAGE_DOWN,     GLFW_KEY_PAGE_DOWN},
    {KEY_LEFT_SHIFT,    GLFW_KEY_LEFT_SHIFT},
    {KEY_RIGHT_SHIFT,   GLFW_KEY_RIGHT_SHIFT},
    {KEY_LEFT_CONTROL,  GLFW_KEY_LEFT_CONTROL},
    {KEY_RIGHT_CONTROL, GLFW_KEY_RIGHT_CONTROL},

    {KEY_1, GLFW_KEY_1}, {KEY_2, GLFW_KEY_2}, {KEY_3, GLFW_KEY_3},
    {KEY_4, GLFW_KEY_4}, {KEY_5, GLFW_KEY_5}, {KEY_6, GLFW_KEY_6},
    {KEY_7, GLFW_KEY_7}, {KEY_8, GLFW_KEY_8}, {KEY_9, GLFW_KEY_9},
    {KEY_0, GLFW_KEY_0},

    {KEY_Q, GLFW_KEY_Q}, {KEY_W, GLFW_KEY_W}, {KEY_E, GLFW_KEY_E},
    {KEY_R, GLFW_KEY_R}, {KEY_T, GLFW_KEY_T}, {KEY_Y, GLFW_KEY_Y},
    {KEY_U, GLFW_KEY_U}, {KEY_I, GLFW_KEY_I}, {KEY_O, GLFW_KEY_O},
    {KEY_P, GLFW_KEY_P},

    {KEY_A, GLFW_KEY_A}, {KEY_S, GLFW_KEY_S}, {KEY_D, GLFW_KEY_D},
    {KEY_F, GLFW_KEY_F}, {KEY_G, GLFW_KEY_G}, {KEY_H, GLFW_KEY_H},
    {KEY_J, GLFW_KEY_J}, {KEY_K, GLFW_KEY_K}, {KEY_L, GLFW_KEY_L},

    {KEY_Z, GLFW_KEY_Z}, {KEY_X, GLFW_KEY_X}, {KEY_C, GLFW_KEY_C},
    {KEY_V, GLFW_KEY_V}, {KEY_B, GLFW_KEY_B}, {KEY_N, GLFW_KEY_N},
    {KEY_M, GLFW_KEY_M},
};

static const mouse_map_t mouse_list[] = {
    {MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_LEFT},
    {MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_MIDDLE},
    {MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_RIGHT},
};

static bool map_inited = false;
static int key_glfw_map[60];
static enum KEYS glfw_key_map[1024];
static int btn_glfw_map[60];
static enum BUTTONS glfw_btn_map[1024];

void map_init(void) {
    if (!map_inited) {
        map_inited = true;
        const void* end;
        end = key_list + sizeof(key_list) / sizeof(key_map_t);
        for (const key_map_t *i = key_list; i != end; i++) {
            key_glfw_map[i->key] = i->glfw_key;
            glfw_key_map[i->glfw_key] = i->key;
        }
        end = mouse_list + sizeof(mouse_list) / sizeof(mouse_map_t);
        for (const mouse_map_t *i = mouse_list; i != end; i++) {
            btn_glfw_map[i->btn] = i->glfw_btn;
            glfw_btn_map[i->glfw_btn] = i->btn;
        }
    }
}

void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

int window_init(Window *win) {
    // glfw: initialize and configure
    // ------------------------------
    map_init();
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "osdo",
                                          NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, win);
    glfwSetFramebufferSizeCallback(window, window_resize_cb);
    glfwSetScrollCallback(window, window_scroll_cb);
    glfwSetCharCallback(window, window_char_cb);
    glfwSetMouseButtonCallback(window, window_mouse_button_cb);
    glfwSetCursorPosCallback(window, window_mouse_motion_cb);
    glfwSetKeyCallback(window, window_key_cb);

    // load glew
    // ---------
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    win->size[0] = SCR_WIDTH;
    win->size[1] = SCR_HEIGHT;
    win->mouse_capute = false;
    win->window = window;
    win->current_time = glfwGetTime();
    win->last_time = win->current_time;
    win->delta_time = 0;
    win->scroll_cb = NULL;
    win->mouse_motion_cb = NULL;
    win->char_cb = NULL;
    win->mouse_button_cb = NULL;
    win->key_cb = NULL;
    return 0;
}

void window_del(UNUSED Window *window) {
    glfwTerminate();
}

bool window_pre_loop(Window *window) {
    if (glfwWindowShouldClose(window->window)) return false;
    glfwMakeContextCurrent(window->window);
    glfwPollEvents();
    window_update_cursor(window);
    glfwGetWindowSize(window->window, window->size, window->size + 1);
    glfwGetFramebufferSize(
                window->window, window->display, window->display + 1);
    window->scale[0] = (float)window->display[0] / (float)window->size[0];
    window->scale[1] = (float)window->display[1] / (float)window->size[1];
    window->current_time = glfwGetTime();
    window->delta_time = window->current_time - window->last_time;
    window->last_time = window->current_time;
    return true;
}

void window_post_loop(Window *window) {
    glfwSwapBuffers(window->window);
}

void window_set_user_pointer(Window *window, void *pointer) {
    window->user_pointer = pointer;
}

void *window_get_user_pointer(Window *window) {
    return window->user_pointer;
}

float window_get_resolution(Window *window) {
    return (float)window->size[0] / (float)window->size[1];
}

double window_get_delta_time(Window *window) {
    return window->delta_time;
}

const char * window_get_clipboard(Window *window) {
     return glfwGetClipboardString(window->window);
}

void window_set_clipboard(Window *window, const char * str) {
    glfwSetClipboardString(window->window, str);
}

bool window_is_mouse_caputed(Window *window) {
    return window->mouse_capute;
}

void window_set_mouse_capute(Window *window, bool capute) {
    window->mouse_capute = capute;
}

void window_grab_mouse(Window *window, bool grab) {
    glfwSetInputMode(window->window, GLFW_CURSOR,
                     grab ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

}

bool window_is_key_pressed(Window *window, enum KEYS key) {
    return glfwGetKey(window->window, key_glfw_map[key]) == GLFW_PRESS;
}

bool window_is_mouse_pressed(Window *window, enum BUTTONS key) {
    return glfwGetMouseButton(
                window->window, btn_glfw_map[key]) == GLFW_PRESS;
}

int *window_get_cursor(Window *window) {
    return window->cursor;
}
void window_update_cursor(Window *window) {
    GLdouble c[2];
    glfwGetCursorPos(window->window, c, c + 1);
    window->cursor[0] = (int)c[0]; window->cursor[1] = (int)c[1];
}

void window_set_cursor(Window *window, int coords[2]) {
    glfwSetCursorPos(window->window, (double)coords[0], (double)coords[1]);
    window->cursor[0] = coords[0];
    window->cursor[1] = coords[1];
}

int *window_get_size(Window *window) {
    return window->size;
}

int *window_get_display(Window *window) {
    return window->display;
}

float *window_get_scale(Window *window) {
    return window->scale;
}

int *window_get_double_click_pos(Window *window) {
    return window->double_click_pos;
}

bool window_is_double_clicked(Window *window) {
    return window->double_click;
}

void window_set_scroll_cb(Window *window, scroll_cb_t callback) {
    window->scroll_cb = callback;
}

void window_set_mouse_motion_cb(
        Window *window, mouse_motion_cb_t callback) {
    window->mouse_motion_cb = callback;
}

void window_set_char_cb(Window *window, char_cb_t callback) {
    window->char_cb = callback;
}
void window_set_mouse_button_cb(
        Window *window, mouse_button_cb_t callback) {
    window->mouse_button_cb = callback;
}

void window_set_key_cb(Window *window, key_cb_t callback) {
    window->key_cb = callback;
}

void window_resize_cb(
        struct GLFWwindow* window, GLint width, GLint height) {
    Window *win = glfwGetWindowUserPointer(window);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    win->size[0] = width; win->size[1] = height;
}

void window_scroll_cb(
        struct GLFWwindow* window, GLdouble xoffset, GLdouble yoffset) {
    Window *win = glfwGetWindowUserPointer(window);
    win->scroll_cb(win, xoffset, yoffset);
}

void window_mouse_motion_cb(
        struct GLFWwindow* window, double xpos, double ypos) {
    Window *win = glfwGetWindowUserPointer(window);
    int offset[] = {(int)xpos - win->cursor[0],
                    (int)ypos - win->cursor[1]};
    win->cursor[0] = (int)xpos; win->cursor[1] = (int)ypos;
    win->mouse_motion_cb(win, offset);
}

void window_char_cb(GLFWwindow* window, unsigned int codepoint) {
    Window *win = glfwGetWindowUserPointer(window);
    win->char_cb(win, codepoint);
}

void window_mouse_button_cb(
        GLFWwindow *window, int button, int action, UNUSED int mods) {
    Window *win = glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)  {
            double dt = glfwGetTime() - win->last_click_time;
            if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {
                win->double_click = true;
                win->double_click_pos[0] = win->cursor[0];
                win->double_click_pos[1] = win->cursor[1];
            }
            win->last_click_time = glfwGetTime();
        } else
            win->double_click = false;
    }
    win->mouse_button_cb(win, glfw_btn_map[button], action == GLFW_PRESS);
}

void window_key_cb(GLFWwindow* window, int key, UNUSED int scancode,
                   int action, UNUSED int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    Window *win = glfwGetWindowUserPointer(window);
    win->key_cb(win, glfw_key_map[key], action == GLFW_PRESS);
}
