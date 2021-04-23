#include "beziatorpanel.h"
#include "imgui.h"

BeziatorPanel::BeziatorPanel(const string &_path)
    : Beziator(_path) {}

void BeziatorPanel::edit_panel()
{
    ImGui::SetNextWindowPos(ImVec2{308, 4}, ImGuiCond_FirstUseEver, ImVec2{0,0});
    ImGui::SetNextWindowSize(ImVec2{300, 600}, ImGuiCond_Always);
    ImGui::Begin("Bezier mesh");
    if (ImGui::Button("Regenerate", ImVec2{0, 0}))
        this->generate();
    if (ImGui::Button("Save", ImVec2{0, 0}))
        this->save();
    ImGui::BeginChild("Control points", ImVec2{ImGui::GetWindowContentRegionWidth(), 260}, true, 0);
    bool changed = false;
    for (size_t i = 0, size = this->vertices.size(); i < size; i++) {
        int id = static_cast<int>(i);
        ImGui::PushID(id);
        changed |= ImGui::DragFloat3("##empty", this->vertices[i].position, 0.01f);
        ImGui::SameLine(0, 5);
        ImGui::Text("%i", id);
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::BeginChild("Surfaces", ImVec2{ImGui::GetWindowContentRegionWidth(), 260}, true, 0);
    const size_t surfaces_size = this->indices.size() / 16;
    surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(indices.data());
    for (size_t i = 0; i < surfaces_size; i++) {
        int id = static_cast<int>(i);
        ImGui::PushID(id);
        ImGui::Text("%i", id);
        if (ImGui::Button("Invert light normals", ImVec2{0, 0})) {
            this->rotate(i);
            this->generate();
        }
        changed |= ImGui::InputScalarN("##first", ImGuiDataType_U32, surfaces[i][0], 4);
        changed |= ImGui::InputScalarN("##second", ImGuiDataType_U32, surfaces[i][1], 4);
        changed |= ImGui::InputScalarN("##third", ImGuiDataType_U32, surfaces[i][2], 4);
        changed |= ImGui::InputScalarN("##fourth", ImGuiDataType_U32, surfaces[i][3], 4);
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::End();
    if (changed)
        this->generate();
}
