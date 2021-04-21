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

/*#include "EASTL/hash_map.h"
#include "EASTL/string.h"
using eastl::hash_map;
using eastl::string;*/

#include <unordered_map>
#include <string>
#include <memory>
using std::unordered_map;
using std::string;
using std::shared_ptr;
using std::make_shared;

enum TRANSFORMATIONS {
    ROTATE    = 0,
    TRANSLATE = 1,
    ANIMATE   = 2,
};

class App {
    unordered_map<string, shared_ptr<Model>> models;
    unordered_map<string, shared_ptr<Shader>> shaders;
    Scene scene;
    vector<Object> objects;
    Camera camera;
    //NkGlfw nkglfw;
    DeImgui deimgui;
    Window window;
    bool interactive_mode;
    int trans[3][3];

    // buffered data for loop
    mat4 mat4buf, projection, last_camera;
    vec4 vec4buf;
public:
    int init();

    void del();

    int loop();

    bool load_shader(const char *name);

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
