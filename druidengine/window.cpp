#include "window.h"
#include "conf.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

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

int Window::init() {
    // glfw: initialize and configure
    // ------------------------------
    map_init();
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "osdo",
                                          nullptr, nullptr);
    if (window == nullptr) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Window::resize_cb);
    glfwSetScrollCallback(window, Window::scroll_cb);
    glfwSetCharCallback(window, Window::char_cb);
    glfwSetMouseButtonCallback(window, Window::mouse_button_cb);
    glfwSetCursorPosCallback(window, Window::mouse_motion_cb);
    glfwSetKeyCallback(window, Window::key_cb);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    this->size[0] = SCR_WIDTH;
    this->size[1] = SCR_HEIGHT;
    this->mouse_capute = false;
    this->window = window;
    this->current_time = glfwGetTime();
    this->last_time = this->current_time;
    this->delta_time = 0;
    this->_scroll_cb = nullptr;
    this->_mouse_motion_cb = nullptr;
    this->_char_cb = nullptr;
    this->_mouse_button_cb = nullptr;
    this->_key_cb = nullptr;
    return 0;
}

Window::~Window() {
    glfwTerminate();
}


GLFWwindow *Window::get() {
    return this->window;
}


bool Window::alive() {
    return !glfwWindowShouldClose(this->window);
}

bool Window::pre_loop() {
    if (glfwWindowShouldClose(this->window)) return false;
    glfwMakeContextCurrent(this->window);
    glfwPollEvents();
    //glfwWaitEvents();
    glfwGetWindowSize(this->window, this->size, this->size + 1);
    glfwGetFramebufferSize(
                this->window, this->display, this->display + 1);
    this->scale[0] = (float)this->display[0] / (float)this->size[0];
    this->scale[1] = (float)this->display[1] / (float)this->size[1];
    this->current_time = glfwGetTime();
    this->delta_time = this->current_time - this->last_time;
    this->last_time = this->current_time;
    return true;
}

void Window::post_loop() {
    glfwSwapBuffers(this->window);
}

void Window::set_user_pointer(void *pointer) {
    this->user_pointer = pointer;
}

void *Window::get_user_pointer() {
    return this->user_pointer;
}

float Window::get_resolution() {
    return (float)this->size[0] / (float)this->size[1];
}

double Window::get_delta_time() {
    return this->delta_time;
}

const char * Window::get_clipboard() {
     return glfwGetClipboardString(this->window);
}

void Window::set_clipboard(const char * str) {
    glfwSetClipboardString(this->window, str);
}

bool Window::is_mouse_caputed() {
    return this->mouse_capute;
}

void Window::grab_mouse(bool grab) {
    if (grab != this->mouse_capute) {
        this->mouse_capute = grab;
        glfwSetInputMode(this->window, GLFW_CURSOR,
                         grab ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

}

bool Window::is_key_pressed(enum KEYS key) {
    return glfwGetKey(this->window, key_glfw_map[key]) == GLFW_PRESS;
}

bool Window::is_mouse_pressed(enum BUTTONS key) {
    return glfwGetMouseButton(
                this->window, btn_glfw_map[key]) == GLFW_PRESS;
}

void Window::get_cursor(vec2 dest) {
    glm_vec2_copy(this->cursor, dest);
}

void Window::set_cursor(vec2 coords) {
    glfwSetCursorPos(this->window, (double)coords[0], (double)coords[1]);
    glm_vec2_copy(coords, this->cursor);
}

int *Window::get_size() {
    return this->size;
}

int *Window::get_display() {
    return this->display;
}

float *Window::get_scale() {
    return this->scale;
}

void Window::set_scroll_cb(scroll_cb_t callback) {
    this->_scroll_cb = callback;
}

void Window::set_mouse_motion_cb(mouse_motion_cb_t callback) {
    this->_mouse_motion_cb = callback;
}

void Window::set_char_cb(char_cb_t callback) {
    this->_char_cb = callback;
}
void Window::set_mouse_button_cb(mouse_button_cb_t callback) {
    this->_mouse_button_cb = callback;
}

void Window::set_key_cb(key_cb_t callback) {
    this->_key_cb = callback;
}

void Window::resize_cb(
        struct GLFWwindow* window, GLint width, GLint height) {
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    win->size[0] = width; win->size[1] = height;
}

void Window::scroll_cb(
        struct GLFWwindow* window, GLdouble xoffset, GLdouble yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win->_scroll_cb != nullptr)
        win->_scroll_cb(win, xoffset, yoffset);
}

void Window::mouse_motion_cb(
        struct GLFWwindow* window, double xpos, double ypos) {
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    vec2 pos = {(float)xpos, (float)ypos}, offset;
    glm_vec2_sub(pos, win->cursor, offset);
    glm_vec2_copy(pos, win->cursor);
    if (win->_mouse_motion_cb)
        win->_mouse_motion_cb(win, pos, offset);
}

void Window::char_cb(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win->_char_cb)
        win->_char_cb(win, codepoint);
}

void Window::mouse_button_cb(
        GLFWwindow *window, int button, int action, UNUSED int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    enum BUTTONS btn = glfw_btn_map[button];
    bool pressed = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT && pressed) {
        double dt = glfwGetTime() - win->last_click_time;
        if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {
            btn = MOUSE_BUTTON_DOUBLE;
        }
        win->last_click_time = glfwGetTime();
    }
    if (pressed || action == GLFW_RELEASE)
        win->_mouse_button_cb(win, btn, pressed);
}

void Window::key_cb(GLFWwindow* window, int key, UNUSED int scancode,
                   int action, UNUSED int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    bool pressed = action == GLFW_PRESS;
    if (pressed || action == GLFW_RELEASE)
        win->_key_cb(win, glfw_key_map[key], pressed);
}
