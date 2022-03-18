//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpu_noise_layers_generator.h"
#include "gui_entry.h"
#include "editor_utils.h"

namespace vox {
namespace editor {
int count = 1;

CPUNoiseLayersGenerator::CPUNoiseLayersGenerator():
uid(generateId(32)),
maskManager(uid) {
    name.reserve(1024);
    name = "CPU Noise Layer " + std::to_string(count++);
}

void CPUNoiseLayersGenerator::showSetting(int id) {
    ImGui::InputText(("Name##CPUNL" + std::to_string(id)).c_str(), name.data(), 1024);
    ImGui::Checkbox(("Enabled##CPUNL" + std::to_string(id)).c_str(), &enabled);
    
    if (ImGui::Button(("Edit##CPUNL" + std::to_string(id)).c_str())) {
        windowStat = true;
    }
    
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader(("Custom Base Mask##GMSK" + uid).c_str())) {
        maskManager.showSettings();
    }
    
    ImGui::Separator();
    ImGui::Text("UID : %s", uid.data());
    ImGui::Text("Time : %lf ms", time);
}

void CPUNoiseLayersGenerator::update() {
    if (windowStat) {
        ImGui::Begin((name + "##" + uid).c_str(), &windowStat);
        noiseManager.render();
        ImGui::End();
    }
    
    if (!GUIEntry::getSingleton().states.remeshing) {
        noiseManager.updateLayers();
    }
}

float CPUNoiseLayersGenerator::evaluateAt(float x, float y, float z) {
    if (maskManager.enabled) {
        return maskManager.evaluateAt(x, y, z, noiseManager.evaluate(x + noiseManager.offset[0],
                                                                     y + noiseManager.offset[1],
                                                                     z + noiseManager.offset[2]));
    } else {
        return noiseManager.evaluate(x, y, z);
    }
}

}
}
