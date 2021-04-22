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

    // build and compile our shader zprogram
    // -------------------------------------
    if (/*!this->load_shader("simple") ||
            !this->load_shader("textured") ||
            !this->load_shader("lighting") ||
            !this->load_shader("nuklear") ||*/
            !this->load_shader("editmode"))
        return -1;

    auto shader_iter = this->shaders.find("editmode");
    if (shader_iter == this->shaders.end())
        return -1;
    shared_ptr<Shader> shader = shader_iter->second;

    {
        auto beziator = make_shared<BeziatorPanel>("car", shader);
        bool success = beziator->init();
        if (!success)
            return -1;
        beziator->generate();
        this->models["car"] = beziator;

        Object object(this->models["car"], shader);
        object.translate_object(vec3{-5, 5, 0});
        object.scale(vec3{4, 4, 4});
        object.rotate_object(M_PI_F / 1.1f, X);
        this->objects.push_back(object);

        auto cube_model = make_shared<Mesh>();
        cube_model->cube_update();
        //this->objects.push_back(Object(cube_model, shader));
    }

    this->scene = Scene(this->objects);
    this->camera.translate_camera(BASIS0POS);

    this->deimgui.init(&this->window);

    return 0;
}

int App::loop() {
    Scene* scene = &this->scene;
    Shader &sh = *this->shaders.find("editmode")->second;
    //struct nk_context *ctx = &this->nkglfw.context;
    //struct nk_colorf bg;
    //bg.r = 0.8f; bg.g = 0.9f; bg.b = 0.8f; bg.a = 1.0f;
    char text[128];
    vec4 *position, direction;
    vec3 rotation, *animation;
    Bijective *bijective;
    bool light = false;
    EasyVector<vec4> points(0);

    Buffer buf, buf2;

    // render loop
    // -----------
    while(this->window.alive()) {
        //nk_glfw_begin_input(&this->nkglfw);
        this->window.pre_loop();
        //nk_glfw_end_input(&this->nkglfw);
        this->deimgui.update();

        if (scene->active)
            snprintf(text, 128, "Object %zu", scene->active);
        else snprintf(text, 128, "Camera");

        if (scene->active) {
            bijective = &scene->objects[scene->active - 1];
        } else {
            bijective = &this->camera;
        }
        bijective->get_position(&position);
        bijective->get_animation(&animation);
        glm_vec3_copy(vec3 GLM_VEC3_ZERO_INIT, rotation);

        // render
        // ------
        glClearColor(1,1,1,1);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        // -----
        App::process_input();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::SetNextWindowPos(ImVec2{4, 4}, ImGuiCond_FirstUseEver, ImVec2{0,0});
        ImGui::SetNextWindowSize(ImVec2{300, 512}, ImGuiCond_FirstUseEver);

        ImGui::Begin("OSDO", nullptr, 0);
        ImGui::Checkbox("Wireframe", &this->scene.wireframe);
        ImGui::Checkbox("Light", &light);
        ImGui::Text("Active element: %s", text);
        {
            const ImU64 u64_zero = 0;
            const ImU64 length = scene->objects.size();
            ImGui::SliderScalar("", ImGuiDataType_U64, &scene->active, &u64_zero, &length, "%i", 0);
        }
        ImGui::InputFloat3("Potision", *position, "%g", 0);
        ImGui::InputFloat3("Rotation", rotation, "%g", 0);
        ImGui::InputFloat3("Animation", *animation, "%g", 0);
        ImGui::End();

        if (scene->active) {
            BeziatorPanel &beziator = static_cast<BeziatorPanel&>(*this->models.begin()->second);
            Object &obj = *scene->objects.begin();

            mat4 matr = GLM_MAT4_IDENTITY_INIT;
            //glm_scale(matr, (vec3){(float)this->window.size[0], (float)this->window.size[1], 1});
            glm_mat4_mul(matr, this->projection, matr);
            glm_mat4_mul(matr, this->last_camera, matr);
            glm_mat4_mul(matr, *(obj.get_transform()), matr);
            beziator.edit_panel(matr);
            ImGui::End();
        }

        ImGui::Begin("Win", nullptr, 0);
        ImVec2 vMin, vMax, size, win_pos;
        win_pos = ImGui::GetWindowPos();
        vMin = ImGui::GetWindowContentRegionMin();
        vMax = ImGui::GetWindowContentRegionMax();
        size.x = max(vMax.x-vMin.x, 5.f); size.y = max(vMax.y-vMin.y, 5.f);

        {
            GLsizei glsize[] = {
                static_cast<GLsizei>(size.x),
                static_cast<GLsizei>(size.y),
            };
            if (buf.pre_render(glsize)) {
                glClearColor(1, 1, 1, 1);
                glClearDepth(1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glActiveTexture(GL_TEXTURE0);

                sh.bind();

                // pass projection matrix to shader
                glm_perspective(45.f * (GLfloat) M_RAD, size.x / size.y,
                                0.01f, 100.0f, this->projection);
                //glm_mat4_identity(this->projection);
                sh.set_mat4("projection", this->projection);
                sh.set_float("materialShininess", 32.0f);
                sh.set_vec3("objectColor", vec3{0,1,0});

                // directional light
                this->camera.get_direction(direction);
                if (!light)
                    sh.set_vec3("dirLight.direction", vec3{0,0,-0.5});
                else
                    sh.set_vec3("dirLight.direction", direction);
                sh.set_vec3f("dirLight.ambient", 0.0f, 0.0f, 0.0f);
                sh.set_vec3f("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
                sh.set_vec3f("dirLight.specular", 0.f, 0.f, 0.f);

                // camera/view transformation
                this->camera.get_mat4(this->last_camera);
                {
                    vec4 *pos;
                    this->camera.get_position(&pos);
                    sh.set_vec3("viewPos", *pos);
                }
                sh.set_vec3f("objectColor", 0.4f, 0.8f, 0.4f);
                sh.set_float("materialShininess", 32.0f);
                sh.set_mat4("camera", this->last_camera);
                {
                    int *size = this->window.get_size();
                    sh.set_vec2("vp", vec2{(float)size[0], (float)size[1]});
                }

                sh.set_float("alpha", 1.0);

                bijective->rotate_all(rotation);

                // Culling unneded back faced
                //glEnable(GL_CULL_FACE);
                //glCullFace(GL_BACK);
                //glFrontFace(GL_CCW);

                //glEnable(GL_BLEND);
                //glBlendEquation(GL_FUNC_ADD);
                //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                //glEnable(GL_ALPHA_TEST);
                //glAlphaFunc(GL_GREATER, 0.0f);

                if (!scene->wireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                // render the loaded models
                for (Object &i : scene->objects) {
                    i.draw(this->mat4buf, this->window.get_delta_time());
                }
            } else {
                printf("Error\n");
            }
            buf.post_render(glsize);
        }


        {
            ImVec2 cursorBegin = ImGui::GetCursorPos();
            ImGui::Image(buf.get_tex().get_vid(), size,
                         ImVec2{0, 0}, ImVec2{1, 1}, ImVec4{1,1,1,1}, ImVec4{0,0,0,0});

            ImGui::SetCursorPos(cursorBegin);
            ImGui::InvisibleButton("##empty", size);
            const bool is_hovered = ImGui::IsItemHovered(0);
            const bool is_active = ImGui::IsItemActive();
            if (is_hovered) {
                ImGuiIO &io = ImGui::GetIO();
                if (is_active) {
                    vec3 mouse_pos = {io.MouseDelta.y / size.y * 2, -io.MouseDelta.x / size.x * 2, 0};
                    this->camera.rotate_all_camera(mouse_pos);
                }
                this->camera.translate(vec3{0, 0, -io.MouseWheel}, 1);
            }
            ImDrawList *dl = ImGui::GetWindowDrawList();
            if (this->scene.active) {
                BeziatorPanel &beziator = static_cast<BeziatorPanel&>(*this->models.begin()->second);
                Object &obj = *scene->objects.begin();
                obj.get_mat4(this->mat4buf);
                mat4 matr = GLM_MAT4_IDENTITY_INIT;
                glm_mat4_mul(this->mat4buf, matr, matr);
                glm_mat4_mul(this->last_camera, matr, matr);
                glm_mat4_mul(this->projection, matr, matr);
                float minz = 0, maxz = 0;
                Beziator::points_vector& bpoints = beziator.get_points();
                const size_t points_size = bpoints.size();
                points = EasyVector<vec4>(points_size);

                for (size_t i = 0; i < points_size; i++) {
                    //glm_mat4_mulv(matr, beziator->points[i], p);
                    glm_project(bpoints[i], matr, vec4{0, 0, size.x, size.y}, points[i]);
                    if (!i) {
                        minz = points[i][2];
                        maxz = points[i][2];
                    } else {
                        if (minz > points[i][2]) minz = points[i][2];
                        if (maxz < points[i][2]) maxz = points[i][2];
                    }
                }
                for (size_t i = 0; i < points_size; i++) {
                    points[i][2] = (maxz - points[i][2]) / (maxz - minz) * 0.6f + 0.4f;
                }
                ImVec2 pos, sz;
                char buf[16];
                for (size_t i = 0; i < points_size; i++) {
                    pos.x = points[i][0] + win_pos.x + cursorBegin.x;
                    pos.y = points[i][1] + win_pos.y + cursorBegin.y;
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0,0,0,points[i][2]});
                    ImGui::SetCursorPos(ImVec2{points[i][0] + cursorBegin.x, points[i][1] + cursorBegin.y});
                    sprintf(buf, "%zu", i);
                    ImGui::CalcTextSize(buf, nullptr, false, 0);
                    dl->AddRectFilled(
                                ImVec2{pos.x-2,pos.y-2}, ImVec2{pos.x+ sz.x+2, pos.y+ sz.y+2},
                                ImGui::GetColorU32(ImVec4{0,1,0,points[i][2]}), 4, ImDrawCornerFlags_All);
                    ImGui::Text("%s", buf);
                    ImGui::PopStyleColor(1);
                }
            }
        }

        ImGui::End();

        if (scene->active) {
            BeziatorPanel &beziator = static_cast<BeziatorPanel&>(*this->models.begin()->second);
            Object &obj = *scene->objects.begin();

            ImGui::SetNextWindowSize(ImVec2{512, 512}, ImGuiCond_FirstUseEver);

            ImGui::Begin("Win2", nullptr, 0);
            ImVec2 vMin, vMax, size, win_pos;
            win_pos = ImGui::GetWindowPos();
            vMin = ImGui::GetWindowContentRegionMin();
            vMax = ImGui::GetWindowContentRegionMax();
            size.x = max(vMax.x-vMin.x, 5.f); size.y = max(vMax.y-vMin.y, 5.f);

            {
                GLsizei glsize[] = {
                    static_cast<GLsizei>(size.x),
                    static_cast<GLsizei>(size.y),
                };

                if (buf2.pre_render(glsize)) {
                    glClearColor(1, 1, 1, 1);
                    glClearDepth(1.0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glActiveTexture(GL_TEXTURE0);

                    sh.bind();

                    glm_perspective(45.f * (GLfloat) M_RAD, size.x / size.y,
                                    0.01f, 100.0f, this->projection);
                    sh.set_mat4("projection", this->projection);
                    sh.set_float("materialShininess", 32.0f);
                    sh.set_vec3("objectColor", vec3{0,1,0});

                    // directional light
                    sh.set_vec3("dirLight.direction", vec3{0,0,-1});
                    sh.set_vec3f("dirLight.ambient", 0.0f, 0.0f, 0.0f);
                    sh.set_vec3f("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
                    sh.set_vec3f("dirLight.specular", 0.f, 0.f, 0.f);

                    // camera/view transformation
                    mat4 mat4buf = GLM_MAT4_IDENTITY_INIT;
                    glm_translate_z(mat4buf, -10);
                    sh.set_vec3("viewPos", vec3{0,0,-10});
                    sh.set_vec3f("objectColor", 0.4f, 0.8f, 0.4f);
                    sh.set_float("materialShininess", 32.0f);
                    sh.set_mat4("camera", mat4buf);

                    sh.set_float("alpha", 1.0);

                    // render the loaded models
                    for (Object &i : scene->objects) {
                        i.draw(this->mat4buf, this->window.get_delta_time());
                    }
                } else {
                    printf("Error\n");
                }
                buf2.post_render(glsize);
            }

            ImVec2 cursorBegin = ImGui::GetCursorPos();
            ImGui::Image(buf2.get_tex().get_vid(), size,
                         ImVec2{0, 0}, ImVec2{1, 1}, ImVec4{1,1,1,1}, ImVec4{0,0,0,0});

            ImGui::SetCursorPos(cursorBegin);
            ImGui::InvisibleButton("##empty", size);
            const bool is_hovered = ImGui::IsItemHovered(0);
            const bool is_active = ImGui::IsItemActive();
            if (is_hovered) {
                ImGuiIO &io = ImGui::GetIO();
                if (is_active) {
                    obj.rotate(X, -io.MouseDelta.y / size.y * 2);
                    obj.rotate(Y, io.MouseDelta.x / size.x * 2);
                }
                //camera_translate_bijective(&this->camera, (vec3){0, 0, -io->MouseWheel}, 1);
            }
            ImDrawList *dl = ImGui::GetWindowDrawList();
            obj.get_mat4(this->mat4buf);
            mat4 matr = GLM_MAT4_IDENTITY_INIT;
            glm_mat4_mul(this->mat4buf, matr, matr);
            mat4 mat4buf = GLM_MAT4_IDENTITY_INIT;
            glm_translate_z(mat4buf, -10);
            glm_mat4_mul(mat4buf, matr, matr);
            glm_mat4_mul(this->projection, matr, matr);
            float minz = 0, maxz = 0;
            Beziator::points_vector& bpoints = beziator.get_points();
            const size_t points_size = bpoints.size();

            for (size_t i = 0; i < points_size; i++) {
                //glm_mat4_mulv(matr, beziator->points[i], p);
                glm_project(bpoints[i], matr, vec4{0, 0, size.x, size.y}, points[i]);
                if (!i) {
                    minz = points[i][2];
                    maxz = points[i][2];
                } else {
                    if (minz > points[i][2]) minz = points[i][2];
                    if (maxz < points[i][2]) maxz = points[i][2];
                }
            }
            for (size_t i = 0; i < points_size; i++) {
                points[i][2] = (maxz - points[i][2]) / (maxz - minz) * 0.6f + 0.4f;
            }
            ImVec2 pos, sz;
            char buf[16];

            for (size_t i = 0; i < points_size; i++) {
                pos.x = points[i][0] + win_pos.x + cursorBegin.x;
                pos.y = points[i][1] + win_pos.y + cursorBegin.y;
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0,0,0,points[i][2]});
                ImGui::SetCursorPos(ImVec2{points[i][0] + cursorBegin.x, points[i][1] + cursorBegin.y});
                sprintf(buf, "%zu", i);
                ImGui::CalcTextSize(buf, nullptr, false, 0);
                dl->AddRectFilled(ImVec2{pos.x-2,pos.y-2}, ImVec2{pos.x+ sz.x+2, pos.y+ sz.y+2},
                                  ImGui::GetColorU32(ImVec4{0,1,0,points[i][2]}), 4, ImDrawCornerFlags_All);
                ImGui::Text("%s", buf);
                ImGui::PopStyleColor(1);
            }
            ImGui::End();
        }

        this->deimgui.render();
        this->window.post_loop();
    }
    return 0;
}

bool App::load_shader(const char *name) {
    auto shader = Shader::create(name);
    if (!shader) {
        printf("Failed to compile shaders.\n");
        return false;
    }
    this->shaders[name] = shader;
    return true;
}

void App::scroll(UNUSED Window* window, UNUSED GLdouble xoffset, UNUSED GLdouble yoffset) {
    //App *app = window_get_user_pointer(window);
    //nk_gflw_scroll_callback(&this->nkglfw, xoffset, yoffset);
}

void App::mouse(Window* window, UNUSED vec2 pos, vec2 offset) {
    App *app = static_cast<App*>(window->get_user_pointer());

    if (app->interactive_mode) {
        vec2 offset_sens = GLM_VEC2_ZERO_INIT;
        glm_vec2_muladds(offset, -SENSITIVITY, offset_sens);
        //offset_sens[0] *= -1;

        app->camera.rotate_camera(offset_sens[0], Y);
        app->camera.rotate_camera(-offset_sens[1], X);
    }
    //nk_glfw_mouse_callback(&this->nkglfw, pos, offset);
}

void App::char_callback(UNUSED Window* window, UNUSED unsigned int codepoint) {
    //App *app = window_get_user_pointer(window);
    //nk_glfw_char_callback(&this->nkglfw, codepoint);
}

void App::mouse_button_callback(
        UNUSED Window *window, UNUSED enum BUTTONS button,
        UNUSED bool pressed) {
    //App *app = window_get_user_pointer(window);
    /*Object *object = (void*)utarray_eltptr(this->objects, 0);
    Model *model = object->model;
    int *size = window_get_size(&this->window);
    int *cursor = window_get_cursor(&this->window);
    vec4 cp;
    mat4 m;
    object_get_mat4(object, m);
    glm_mat4_mul(this->last_camera, m, m);
    glm_mat4_mul(this->projection, m, m);
    for (size_t i = 0; i < model->points_size; i++) {
        glm_mat4_mulv(m, model->points[i], cp);
        printf("%f %f %f\n", cp[0] / cp[2], cp[1] / cp[2], cp[2]);
    }
    printf("Tap: %f %f\n", (float)cursor[0] * 2.0 / (float)size[0] - 1.0,
        (float)cursor[1] * 2.0 / (float)size[1] - 1.0);*/
    //nk_glfw_mouse_button_callback(&this->nkglfw, button, pressed);
}

// glfw: when the keyboard was used, this callback is called
// ------------------------------------------------------------------
void App::key(Window* window, enum KEYS key, bool pressed) {
    App *app = static_cast<App*>(window->get_user_pointer());
    Scene *scene = &app->scene;
    int t = pressed ? 1 : -1;
    if (pressed) {
        switch (key) {
        case KEY_TAB:
            scene->active++;
            break;
        case KEY_Z:
            scene->wireframe = !scene->wireframe;
            break;
        case KEY_X:
            scene->light = (scene->light)? 0 : 1;
            break;
        case KEY_1:
            scene->dirLightOn = (scene->dirLightOn)? 0 : 1;
            break;
        case KEY_2:
            scene->pointLight1On = (scene->pointLight1On)? 0 : 1;
            break;
        case KEY_3:
            scene->pointLight2On = (scene->pointLight2On)? 0 : 1;
            break;
        case KEY_4:
            scene->spotLightOn = (scene->spotLightOn)? 0 : 1;
            break;
        case KEY_0:
            scene->dirLightOn = 0;
            scene->pointLight1On = 0;
            scene->pointLight2On = 0;
            scene->spotLightOn = 0;
            break;
        case KEY_M:
            app->scene = Scene(app->objects);
            break;
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
    if (scene->active > scene->objects.size()) scene->active = 0;
    //nk_glfw_key_callback(&this->nkglfw, key, pressed);
}

static mat3 m3i = GLM_MAT3_IDENTITY_INIT;

void App::process_input() {
    Window *window = &this->window;
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
    }
}
