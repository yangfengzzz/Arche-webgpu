//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "clear_mesh_generator.h"
#include "gui_entry.h"

namespace vox {
namespace editor {
void ClearMeshGenerator::generate() {
    auto appState = GUIEntry::getSingletonPtr();
    
    if (useGPU) {
    } else {
        if (appState->mode == ApplicationMode::TERRAIN) {
        } else if (appState->mode == ApplicationMode::CUSTOM_BASE) {
        }
    }
}


void ClearMeshGenerator::showSettings() {
    if (ImGui::Checkbox("Use GPU##CMG", &useGPU)) {
    }

    ImGui::Checkbox("Use GPU For Normals(Flat Shading)##CMG", &GUIEntry::getSingletonPtr()->states.useGPUForNormals);
    ImGui::Text("Time : %lf ms", time);
}
}
}
