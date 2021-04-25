#include "float.h"
#include "app.h"
#include "conf.h"
#include "shader.h"
#include "beziatorpanel.h"
#include <imgui.h>
#include "buffer.h"
#include "ImFileDialog.h"
#include "image.h"

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

    auto shader_paths = create_shader_paths("bezier");
    auto shader_adv_paths = create_adv_shader_paths("bezier");
    if (!this->context.load_shader("main", shader_paths) ||
            !this->context.load_shader("main_adv", shader_adv_paths)) {
        printf("Failed to compile shaders.\n");
        return -1;
    }

    {
        string path = create_model_paths("car");
        if (load_model(path)) {
            auto& object = this->context.models[path];
            object.translate_object(vec3{-5, 5, 0});
            object.scale(vec3{4, 4, 4});
            object.rotate_object(M_PI_F / 1.1f, X);
        }
    }
    scenes.emplace_back(Scene::create(context.models));
    add_subwindow(scenes.back());

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

        if (ifd::FileDialog::Instance().IsDone("ModelOpenDialog")) {
            if (ifd::FileDialog::Instance().HasResult()) {
                string res = ifd::FileDialog::Instance().GetResult().c_str();
                load_model(res);
            }
            ifd::FileDialog::Instance().Close();
        }

        if (ifd::FileDialog::Instance().IsDone("TextureOpenDialog")) {
            if (ifd::FileDialog::Instance().HasResult()) {
                string res = ifd::FileDialog::Instance().GetResult().c_str();
                context.load_texture(res.c_str());
            }
            ifd::FileDialog::Instance().Close();
        }

        if (context.active != context.models.end()) {
            context.active->second.get_model()->edit_panel();
        }

        ImGui::SetNextWindowPos(ImVec2{4, 4}, ImGuiCond_FirstUseEver, ImVec2{0,0});
        ImGui::SetNextWindowSize(ImVec2{300, 512}, ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Textures")) {
            if (ImGui::Button("Open texture")) {
                ifd::FileDialog::Instance().Open(
                            "TextureOpenDialog", "Open a texture",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");
            }
            auto& models = context.models;
            auto flags = ImGuiTableFlags_SizingStretchProp;
            if (ImGui::BeginTable("textures_table", 2, flags)) {
                ImGui::TableSetupColumn("Texture name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Texture action", ImGuiTableColumnFlags_WidthFixed, -1);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                auto &textures = context.textures;
                if (ImGui::Selectable("None", context.active_texture == textures.end()))
                    context.active_texture = textures.end();
                ImGui::TableNextColumn();
                for (auto it = textures.begin(), end = textures.end(); it != end;) {
                    ImGui::PushID(&*it);
                    char buf[256];
                    snprintf(buf, 256, "Texture: \"%s\"", it->first.c_str());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(buf, context.active_texture == it))
                       context.active_texture = it;
                    ImGui::TableNextColumn();
                    bool erase = ImGui::SmallButton("-");
                    if (erase) {
                        if (context.active_texture == it)
                            context.active_texture = textures.end();
                        it = textures.erase(it);
                    }  else ++it;
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Models")) {
            if (ImGui::Button("Next"))
                context.next_active();
            ImGui::SameLine();
            if (ImGui::Button("Add cube")) {
                auto cube_model = make_shared<Mesh>();
                cube_model->cube_update();
                this->context.models["cube"] = Object(cube_model);
            }
            auto& models = context.models;
            auto flags = ImGuiTableFlags_SizingStretchProp;
            if (ImGui::BeginTable("models_table", 2, flags)) {
                ImGui::TableSetupColumn("Model name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Model action", ImGuiTableColumnFlags_WidthFixed, -1);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable("None", context.active == models.end()))
                    context.active = context.models.end();
                ImGui::TableNextColumn();
                for (auto it = models.begin(), end = models.end(); it != end;) {
                    ImGui::PushID(&*it);
                    char buf[256];
                    snprintf(buf, 256, "Model: \"%s\"", it->first.c_str());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(buf, context.active == it))
                       context.active = it;
                    ImGui::TableNextColumn();
                    bool erase = ImGui::SmallButton("-");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("+")) {
                        auto s = Scene::create(
                                    Context::Models {{it->first, it->second}});
                        scenes.emplace_back(s);
                        add_subwindow(scenes.back());
                    }
                    if (erase)
                       it = models.erase(it);
                    else ++it;
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
        if (ImGui::Begin("Scenes")) {
            auto flags = ImGuiTableFlags_SizingStretchProp;
            if (ImGui::BeginTable("scenes_table", 2, flags)) {
                ImGui::TableSetupColumn("Scene name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Scene action", ImGuiTableColumnFlags_WidthFixed, -1);
                int i = 0, j;
                for (auto it = scenes.begin(), end = scenes.end(); it != end;) {
                    ImGui::PushID(&*it);
                    char buf[256];
                    snprintf(buf, 256, "Scene %i", i++);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::TreeNode(buf)) {
                        j = 0;
                        auto& objects = (*it)->objects;
                        for (auto i = objects.begin(), endi = objects.end(); i != endi;) {
                            ImGui::PushID(&*i);
                            Object &obj = i->second;
                            snprintf(buf, 256, "Object: \"%s\"", i->first.c_str());
                            bool erase = false;
                            if (ImGui::TreeNode(buf)) {
                                erase = ImGui::SmallButton("-");
                                object_edit(obj);
                                ImGui::TreePop();
                            }
                            if (erase)
                                i = objects.erase(i);
                            else
                                ++i;
                            ImGui::PopID();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TableNextColumn();
                    bool erase = ImGui::SmallButton("-");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("+")) {
                        add_subwindow(*it);
                    }
                    if (erase)
                        it = close_scene(it);
                    else ++it;
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();

        size_t id = 0;

        for (auto i = subwindows.begin(), end = subwindows.end(); i != end;) {
            id++;
            if (!(*i)->loop(id, window.get_delta_time()))
                i = subwindows.erase(i);
            else i++;
        }

        this->deimgui.render();
        this->window.post_loop();
    }
    return 0;
}

string get_path_from_name(const char *name, const char *pattern) {
    string s;
    s.resize(strlen(name) + strlen(pattern));
    snprintf(&s[0], s.size(), pattern, name);
    return s;
}

Shader::shader_map App::create_shader_paths(const char *name) {
    return {
        {VERT_SHADER, get_path_from_name(name, VERTEX_PATH)},
        {FRAG_SHADER, get_path_from_name(name, FRAGMENT_PATH)},
    };
}

Shader::shader_map App::create_adv_shader_paths(const char *name) {
    return {
        {VERT_SHADER, get_path_from_name(name, VERTEX_PATH)},
        {TESC_SHADER, get_path_from_name(name, TESC_PATH)},
        {TESE_SHADER, get_path_from_name(name, TESE_PATH)},
        {FRAG_SHADER, get_path_from_name(name, FRAGMENT_PATH)},
    };
}

string App::create_model_paths(const char *name)
{
    return get_path_from_name(name, BEZIATOR_PATH);
}

bool App::load_model(const string &path) {
    if (this->context.models.find(path) != this->context.models.end())
        return false;

    auto beziator = make_shared<BeziatorPanel>(path);

    if (!beziator || !beziator->init())
        return false;

    beziator->generate();

    Object object(beziator);
    this->context.models[path] = object;
    return true;
}

void App::add_subwindow(shared_ptr<Scene>& scene) {
    auto win = make_shared<SubWindow>(window, context, scene);
    subwindows.emplace_back(win);
    winmap[scene].emplace(make_pair(&subwindows.back(), --subwindows.end()));
}

App::SubWinIter App::remove_subwindow(SubWinIter it) {
    winmap[(*it)->get_scene()].erase(&*it);
    return subwindows.erase(it);
}

list<shared_ptr<Scene>>::iterator
App::close_scene(list<shared_ptr<Scene>>::iterator it) {
    auto wins = winmap[*it];
    for (auto& i : wins) {
        remove_subwindow(i.second);
    }
    return scenes.erase(it);
}

void App::object_edit(Object &object) {
    vec4 vector;

    object.get_position(vector);
    if (ImGui::DragFloat3("Potision", vector, 0.1f, 0, 0)) {
        object.set_position(vector);
    }

    object.get_rotation(vector);
    if (ImGui::DragFloat3("Rotation", vector, 0.01f, 0, 0)) {
        object.set_rotation(vector);
    }

    object.get_animation(vector);
    if (ImGui::DragFloat3("Animation", vector, 0.01f, 0, 0)) {
        object.set_animation(vector);
    }
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
