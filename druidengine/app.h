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
#include "EASTL/set.h"
using eastl::list;
using eastl::string;
using eastl::pair;
using eastl::set;

enum TRANSFORMATIONS {
    ROTATE    = 0,
    TRANSLATE = 1,
    ANIMATE   = 2,
};

class App {
    typedef list<shared_ptr<SubWindow>> SubWindows;
    typedef SubWindows::iterator SubWinIter;
    Context context;
    Window window;
    list<shared_ptr<Scene>> scenes;
    SubWindows subwindows;
    hash_map<shared_ptr<Scene>, hash_map<SubWindows::pointer, SubWinIter>> winmap;
    DeImgui deimgui;
    bool interactive_mode;
    int trans[3][3];
public:
    int init();

    int loop();

    pair<string, string> create_shader_paths(const char * name);
    string create_model_paths(const char * name);

    bool load_model(const string &path);

    void add_subwindow(shared_ptr<Scene>& scene);
    SubWinIter remove_subwindow(SubWinIter it);
    list<shared_ptr<Scene>>::iterator
    close_scene(list<shared_ptr<Scene>>::iterator it);

    void object_edit(Object& object);

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
