//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "canvas.h"
#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_wgpu.h"

namespace vox {
namespace ui {
void Canvas::draw() {
    if (!_panels.empty()) {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if (_isDockspace) {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            
            ImGui::Begin("##dockspace", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                         | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
                         | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
            ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::SetWindowPos({0.f, 0.f});
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImGui::SetWindowSize({(float) displaySize.x, (float) displaySize.y});
            ImGui::End();
            
            ImGui::PopStyleVar(3);
        }
        
        for (auto &panel: _panels)
            panel.get().draw();
        
        ImGui::Render();
    }
}

void Canvas::addPanel(Panel &p_panel) {
    _panels.push_back(std::ref(p_panel));
}

void Canvas::removePanel(Panel &p_panel) {
    _panels.erase(std::remove_if(_panels.begin(), _panels.end(),
                                 [&p_panel](std::reference_wrapper<Panel> &p_item) {
        return &p_panel == &p_item.get();
    }));
}

void Canvas::removeAllPanels() {
    _panels.clear();
}

void Canvas::makeDockspace(bool p_state) {
    _isDockspace = p_state;
}

bool Canvas::isDockspace() const {
    return _isDockspace;
}


}
}
