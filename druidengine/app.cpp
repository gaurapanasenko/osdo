#include "float.h"
#include "app.h"
#include "conf.h"
#include "shader.h"
#include "beziatorpanel.h"
#include <imgui.h>
#include "buffer.h"

#include <EASTL/algorithm.h>
using eastl::max;

int App::init() {
    this->interactive_mode = false;
    if (this->window.init())
        return -1;
    memset(this->trans, 0, sizeof(this->trans));

    this->window.set_user_pointer(this);
    this->window.set_scroll_cb(App::scroll);
    this->window.set_mouse_motion_cb(App::mouse);
    this->window.set_char_cb(App::char_callback);
    this->window.set_mouse_button_cb(App::mouse_button_callback);
    this->window.set_key_cb(App::key);

    auto shader_paths = create_shader_paths("editmode");
    if (!this->context.load_shader("main", shader_paths.first.c_str(),
                                   shader_paths.second.c_str())) {
        printf("Failed to compile shaders.\n");
        return -1;
    }

    {
        auto beziator = make_shared<BeziatorPanel>("car");
        bool success = beziator->init();
        if (!success)
            return -1;
        beziator->generate();

        Object object(beziator);
        object.translate_object(vec3{-5, 5, 0});
        object.scale(vec3{4, 4, 4});
        object.rotate_object(M_PI_F / 1.1f, X);
        this->context.models["car"] = object;

        auto cube_model = make_shared<Mesh>();
        cube_model->cube_update();
        //this->context.models["cube"] = Object(cube_model);
    }
    scenes.push_back(Scene(context.models));
    subwindows.emplace_back(make_shared<SubWindow>(context, scenes[0]));

    this->deimgui.init(&this->window);

    return 0;
}

int App::loop() {
    // render loop
    // -----------
    while(this->window.alive()) {
        this->window.pre_loop();
        this->deimgui.update();

        // render
        // ------
        glClearColor(1,1,1,1);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        App::process_input();

        ImGui::SetNextWindowPos(ImVec2{4, 4}, ImGuiCond_FirstUseEver, ImVec2{0,0});
        ImGui::SetNextWindowSize(ImVec2{300, 512}, ImGuiCond_FirstUseEver);

        ImGui::Begin("OSDO", nullptr, 0);
        if (ImGui::Button("Switch active model"))
            context.next_active();
        ImGui::End();
        size_t id = 0;

        for (auto i = subwindows.begin(), end = subwindows.end(); i != end; i++) {
            id++;
            (*i)->loop(id, window.get_delta_time());
        }

        this->deimgui.render();
        this->window.post_loop();
    }
    return 0;
}

pair<string, string> App::create_shader_paths(const char *name) {
    const size_t len = strlen(name);
    string vertex_path, fragment_path;
    vertex_path.resize(len + strlen(VERTEX_PATH));
    fragment_path.resize(len + strlen(FRAGMENT_PATH));
    snprintf(&vertex_path[0], vertex_path.size(), VERTEX_PATH, name);
    snprintf(&fragment_path[0], fragment_path.size(), FRAGMENT_PATH, name);
    return {vertex_path, fragment_path};
}

void App::scroll(UNUSED Window* window, UNUSED GLdouble xoffset,
                 UNUSED GLdouble yoffset) {}

void App::mouse(Window* window, UNUSED vec2 pos, vec2 offset) {
    App *app = static_cast<App*>(window->get_user_pointer());

    if (app->interactive_mode) {
        vec2 offset_sens = GLM_VEC2_ZERO_INIT;
        glm_vec2_muladds(offset, -SENSITIVITY, offset_sens);
        //offset_sens[0] *= -1;

        //!!!app->camera.rotate_camera(offset_sens[0], Y);
        //!!!app->camera.rotate_camera(-offset_sens[1], X);
    }
}

void App::char_callback(UNUSED Window* window, UNUSED unsigned int codepoint) {}

void App::mouse_button_callback(
        UNUSED Window *window, UNUSED enum BUTTONS button,
        UNUSED bool pressed) {}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void App::key(Window* window, enum KEYS key, bool pressed) {
    App *app = static_cast<App*>(window->get_user_pointer());
    int t = pressed ? 1 : -1;
    if (pressed) {
        switch (key) {
        case KEY_B:
        {
            app->interactive_mode = !app->interactive_mode;
            if (!app->interactive_mode)
                window->grab_mouse(false);
            else
                window->grab_mouse( true);
        }
            break;
        default:
            break;
        }
    }
    switch (key) {
    case KEY_Q: app->trans[TRANSLATE][Y] -= t; break;
    case KEY_A: app->trans[TRANSLATE][X] -= t; break;
    case KEY_W: app->trans[TRANSLATE][Z] -= t; break;
    case KEY_S: app->trans[TRANSLATE][Z] += t; break;
    case KEY_E: app->trans[TRANSLATE][Y] += t; break;
    case KEY_D: app->trans[TRANSLATE][X] += t; break;
    case KEY_U: app->trans[ROTATE   ][Z] += t; break;
    case KEY_J: app->trans[ROTATE   ][Y] -= t; break;
    case KEY_I: app->trans[ROTATE   ][X] -= t; break;
    case KEY_K: app->trans[ROTATE   ][X] += t; break;
    case KEY_O: app->trans[ROTATE   ][Z] -= t; break;
    case KEY_L: app->trans[ROTATE   ][Y] += t; break;
    case KEY_R: app->trans[ANIMATE  ][Z] += t; break;
    case KEY_F: app->trans[ANIMATE  ][Y] -= t; break;
    case KEY_T: app->trans[ANIMATE  ][X] -= t; break;
    case KEY_G: app->trans[ANIMATE  ][X] += t; break;
    case KEY_Y: app->trans[ANIMATE  ][Z] -= t; break;
    case KEY_H: app->trans[ANIMATE  ][Y] += t; break;
    default:
        break;

    }
}

//static mat3 m3i = GLM_MAT3_IDENTITY_INIT;

void App::process_input() {
    /*Window *window = &this->window;
    Scene *scene = &this->scene;
    Bijective *bijective;
    float t;
    GLfloat delta_time = (GLfloat)window->get_delta_time();
    if (scene->active) {
        bijective = &scene->objects[scene->active - 1];
    } else {
        bijective = &this->camera;
    }

    if (window->is_key_pressed(KEY_LEFT_CONTROL))
        delta_time *= 10;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (this->trans[i][j]) {
                t = (float)this->trans[i][j] * delta_time;
                switch (i) {
                case TRANSLATE:
                    bijective->translate(m3i[j], t);
                    break;
                case ROTATE:
                    bijective->rotate((enum coord_enum)j, t);
                    break;
                case ANIMATE:
                    bijective->set_animation(m3i[j], t);
                    break;
                }
            }
        }
    }*/
}
