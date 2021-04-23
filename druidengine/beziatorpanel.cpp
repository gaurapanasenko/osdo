#include "beziatorpanel.h"
#include "imgui.h"

BeziatorPanel::BeziatorPanel(const char *name)
    : Beziator(name)
{

}

void BeziatorPanel::edit_panel(mat4 matr)
{
    vec4 p;
    int active_elem = -1;
    float min_d = FLT_MAX, cur_d;

    for (size_t i = 0, size = this->points.size(); i < size; i++) {
        glm_mat4_mulv(matr, this->points[i].position, p);
        cur_d = glm_vec3_distance2(vec3 GLM_VEC3_ZERO_INIT, p);
        if (min_d > cur_d) {
            min_d = cur_d;
            active_elem = static_cast<int>(i);
        }
    }

    ImGui::SetNextWindowPos(ImVec2{4, 4}, ImGuiCond_Always, ImVec2{0,0});
    ImGui::BeginTooltip();
    ImGui::Text("%i", active_elem);
    ImGui::EndTooltip();

    ImGui::SetNextWindowPos(ImVec2{308, 4}, ImGuiCond_FirstUseEver, ImVec2{0,0});
    ImGui::SetNextWindowSize(ImVec2{300, 600}, ImGuiCond_Always);
    ImGui::Begin("Bezier mesh", nullptr, 0);
    if (ImGui::Button("Regenerate", ImVec2{0, 0}))
        this->generate();
    if (ImGui::Button("Save", ImVec2{0, 0}))
        this->save();
    ImGui::BeginChild("Control points", ImVec2{ImGui::GetWindowContentRegionWidth(), 260}, true, 0);
    bool changed = false;
    for (size_t i = 0, size = this->points.size(); i < size; i++) {
        int id = static_cast<int>(i);
        ImGui::PushID(id);
        changed |= ImGui::InputFloat3("##empty", this->points[i].position, "%g", 0);
        ImGui::SameLine(0, 5);
        ImGui::Text("%i", id);
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::BeginChild("Surfaces", ImVec2{ImGui::GetWindowContentRegionWidth(), 260}, true, 0);
    for (size_t i = 0; i < this->surfaces.size(); i++) {
        int id = static_cast<int>(i);
        ImGui::PushID(id);
        ImGui::Text("%i", id);
        if (ImGui::Button("Invert light normals", ImVec2{0, 0})) {
            this->rotate(i);
            this->generate();
        }
        changed |= ImGui::InputScalarN("##first", ImGuiDataType_U64, this->surfaces[i][0], 4);
        changed |= ImGui::InputScalarN("##second", ImGuiDataType_U64, this->surfaces[i][1], 4);
        changed |= ImGui::InputScalarN("##third", ImGuiDataType_U64, this->surfaces[i][2], 4);
        changed |= ImGui::InputScalarN("##fourth", ImGuiDataType_U64, this->surfaces[i][3], 4);
        ImGui::PopID();
    }
    ImGui::EndChild();
    if (changed)
        this->generate();
}
