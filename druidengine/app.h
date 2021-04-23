#ifndef APP_H
#define APP_H

#include "osdo.h"
#include "conf.h"

#include "shader.h"
#include "mesh.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "window.h"
#include "deimgui.h"
#include "context.h"
#include "subwindow.h"
#include "EASTL/list.h"
using eastl::list;
using eastl::string;
using eastl::pair;

enum TRANSFORMATIONS {
    ROTATE    = 0,
    TRANSLATE = 1,
    ANIMATE   = 2,
};

class App {
    Context context;
    vector<Scene> scenes;
    Window window;
    list<shared_ptr<SubWindow>> subwindows;
    DeImgui deimgui;
    bool interactive_mode;
    int trans[3][3];
public:
    int init();

    int loop();

    pair<string, string> create_shader_paths(const char * name);

    static void scroll(Window* window, GLdouble xoffset, GLdouble yoffset);
    static void mouse(Window* window, vec2 pos, vec2 offset);
    static void char_callback(Window* window, unsigned int codepoint);
    static void mouse_button_callback(
            Window *window, enum BUTTONS button, bool pressed);
    static void key(Window* window, enum KEYS key, bool pressed);

    // process all input
    // -----------------
    void process_input();
};

#endif // APP_H
