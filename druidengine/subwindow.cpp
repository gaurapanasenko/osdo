#include "subwindow.h"

SubWindow::SubWindow(Context &context, Scene &scene)
    : wireframe(false),
      light_mode(false),
      culling(false),
      transparency(false),
      camera_mode(false),
      context(context),
      scene(scene)
{
    this->camera.translate_camera(BASIS0POS);
}

bool SubWindow::params_window() {
    ImGui::Checkbox("Wireframe", &this->wireframe);
    ImGui::Checkbox("Light", &this->light_mode);
    ImGui::Checkbox("Culling", &this->culling);
    ImGui::Checkbox("Transparency", &this->transparency);
    ImGui::Checkbox("Camera mode", &this->camera_mode);
    ImGui::Text("Camera");
    vec4 vector;

    camera.get_position(vector);
    if (ImGui::DragFloat3("Potision", vector, 0.1f, 0, 0)) {
        camera.set_position(vector);
    }

    camera.get_rotation(vector);
    if (ImGui::DragFloat3("Rotation", vector, 0.01f, 0, 0)) {
        camera.set_rotation(vector);
    }
    return true;
}

bool SubWindow::render_window(double delta_time) {
    ImVec2 vMin, vMax, win_pos;
    win_pos = ImGui::GetWindowPos();
    vMin = ImGui::GetWindowContentRegionMin();
    vMax = ImGui::GetWindowContentRegionMax();
    ImVec2 size = {
        max(vMax.x-vMin.x, 5.f),
        max(vMax.y-vMin.y, 5.f)
    };

    GLsizei glsize[] = {
        static_cast<GLsizei>(size.x),
        static_cast<GLsizei>(size.y),
    };
    Shader &sh = *context.shaders.find("main")->second;
    if (buffer.pre_render(glsize)) {
        glClearColor(1, 1, 1, 1);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sh.bind();

        // pass projection matrix to shader
        glm_perspective(45.f * M_RAD_F, size.x / size.y,
                        0.1f, 100.0f, this->projection);
        sh.set_mat4("projection", this->projection);
        sh.set_float("materialShininess", 32.0f);
        sh.set_vec3("objectColor", vec3{0,1,0});

        // directional light
        vec4 direction = {0,0,-0.5,1};
        if (this->light_mode)
            this->camera.get_direction(direction);
        sh.set_vec3("dirLight.direction", direction);
        sh.set_vec3f("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        sh.set_vec3f("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        sh.set_vec3f("dirLight.specular", 0.f, 0.f, 0.f);

        // camera/view transformation
        this->camera.get_mat4(this->last_camera);
        {
            vec4 pos;
            this->camera.get_position(pos);
            sh.set_vec3("viewPos", pos);
        }
        sh.set_vec3f("objectColor", 0.4f, 0.8f, 0.4f);
        sh.set_float("materialShininess", 32.0f);
        sh.set_mat4("camera", this->last_camera);
        sh.set_vec2("vp", vec2 {size.x, size.y});

        sh.set_float("alpha", 1.0);

        if (culling) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        } else {
            glDisable(GL_CULL_FACE);
        }

        if (transparency) {
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (auto &i : scene.objects) {
            mat4 mat4buf;
            i.second.draw(sh, mat4buf, delta_time);
        }
    }
    buffer.post_render(glsize);

    ImVec2 cursorBegin = ImGui::GetCursorPos();
    ImGui::Image(buffer.get_tex().get_vid(), size,
                 ImVec2{0, 0}, ImVec2{1, 1}, ImVec4{1,1,1,1}, ImVec4{0,0,0,0});

    ImGui::SetCursorPos(cursorBegin);
    ImGui::InvisibleButton("##empty", size);
    const bool is_hovered = ImGui::IsItemHovered(0);
    const bool is_active = ImGui::IsItemActive();
    if (is_hovered) {
        ImGuiIO &io = ImGui::GetIO();
        if (is_active) {
            vec3 mouse_pos = {
                io.MouseDelta.y / size.y * 2, -io.MouseDelta.x / size.x * 2, 0
            };
            this->camera.rotate_all_camera(mouse_pos);

            //obj.rotate(X, -io.MouseDelta.y / size.y * 2);
            //obj.rotate(Y, io.MouseDelta.x / size.x * 2);
        }
        this->camera.translate(vec3{0, 0, -io.MouseWheel}, 1);
    }


    ImDrawList *dl = ImGui::GetWindowDrawList();
    auto active = this->context.active;
    if (active != this->context.models.end()) {
        shared_ptr<Model> model = active->second.get_model();
        for (auto &i : scene.objects) {
            if (i.second.get_model() != model) continue;
            mat4 mat4buf;
            i.second.get_mat4(mat4buf);
            mat4 matr = GLM_MAT4_IDENTITY_INIT;
            glm_mat4_mul(mat4buf, matr, matr);
            glm_mat4_mul(this->last_camera, matr, matr);
            glm_mat4_mul(this->projection, matr, matr);
            float minz = 0, maxz = 0;
            auto& vertices = model->get_vertices();
            const size_t vertices_size = vertices.size();
            vector<vec4> points(vertices_size);

            for (size_t i = 0; i < vertices_size; i++) {
                glm_project(vertices[i].position, matr,
                            vec4{0, 0, size.x, size.y}, points[i]);
                if (!i) {
                    minz = points[i][2];
                    maxz = points[i][2];
                } else {
                    if (minz > points[i][2]) minz = points[i][2];
                    if (maxz < points[i][2]) maxz = points[i][2];
                }
            }
            for (size_t i = 0; i < vertices_size; i++) {
                points[i][2] = (maxz - points[i][2]) / (maxz - minz) * 0.6f + 0.4f;
            }
            ImVec2 pos, sz;
            char buf[16];
            for (size_t i = 0; i < vertices_size; i++) {
                pos.x = points[i][0] + win_pos.x + cursorBegin.x;
                pos.y = points[i][1] + win_pos.y + cursorBegin.y;
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0,0,0,points[i][2]});
                ImGui::SetCursorPos(ImVec2{points[i][0] + cursorBegin.x,
                                           points[i][1] + cursorBegin.y});
                sprintf(buf, "%lu", i);
                sz = ImGui::CalcTextSize(buf, nullptr, false, 0);
                dl->AddRectFilled(
                            ImVec2{pos.x-2,pos.y-2},
                            ImVec2{pos.x+ sz.x+2, pos.y+ sz.y+2},
                            ImGui::GetColorU32(ImVec4{0,1,0,points[i][2]}),
                            4, ImDrawCornerFlags_All);
                ImGui::Text("%s", buf);
                ImGui::PopStyleColor(1);
            }
        }
    }
    return true;
}

bool SubWindow::loop(size_t id, double delta_time) {
    char name[128];
    snprintf(name, 128, "Subwindow params %lu", id);
    if (ImGui::Begin(name)) {
        params_window();
    }
    ImGui::End();

    snprintf(name, 128, "Subwindow %lu", id);
    ImGui::SetNextWindowSize(ImVec2{512, 512}, ImGuiCond_FirstUseEver);
    auto flags = ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse;
    if (ImGui::Begin(name, nullptr, flags)) {
        render_window(delta_time);
    }
    ImGui::End();
    return true;
}
