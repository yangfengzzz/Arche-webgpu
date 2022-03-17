//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "layered_noise_manager.h"
#include "gui/imgui.h"
#include <iostream>

#define MAKE_IMGUI_ID(x) ("##LayeredNoiseManager" + std::string(x)).c_str()

namespace vox {
namespace editor {
LayeredNoiseManager::LayeredNoiseManager() {
    offset[0] = offset[1] = offset[2] = 0;
    strength = 1.0f;
    absv = false;
    sq = false;
    noiseLayers.emplace_back(std::make_unique<NoiseLayer>());
}

void LayeredNoiseManager::render() {
    ImGui::Text("Global Offsets:");
    ImGui::DragFloat3(MAKE_IMGUI_ID("Global Offsets"), offset, 0.1f);
    ImGui::DragFloat("Global Strength##LayeredNoiseManager", &strength, 0.1f);
    ImGui::Checkbox("Absolute Value##LayeredNoiseManager", &absv);
    ImGui::Checkbox("Square Value##LayeredNoiseManager", &sq);
    ImGui::NewLine();
    ImGui::Text("Noise Layers");
    
    for (int i = 0; i < noiseLayers.size(); i++) {
        bool state = ImGui::CollapsingHeader((std::string("##noiseLayerName") + std::to_string(i)).c_str());
        
        if (state) {
            noiseLayers[i]->render(i);
            
            if (toAdd.size() == 0) {
                if (ImGui::Button("Duplicate")) {
                    toAdd.emplace_back(std::make_unique<NoiseLayer>());
                }
            }
            
            if (noiseLayers.size() > 1 && toDelete.size() == 0) {
                ImGui::SameLine();
                
                if (ImGui::Button("Delete")) {
                    toDelete.push_back(i);
                }
            }
        } else {
            ImGui::SameLine();
            ImGui::Text(noiseLayers[i]->name.c_str());
        }
    }
    
    if (toAdd.size() == 0) {
        if (ImGui::Button("Add New Layer")) {
            toAdd.emplace_back(std::make_unique<NoiseLayer>());
        }
    }
}

void LayeredNoiseManager::updateLayers() {
    if (toDelete.size() > 0) {
        noiseLayers.erase(noiseLayers.begin() + toDelete[0]);
        toDelete.clear();
    }
    
    if (toAdd.size() > 0) {
        noiseLayers.emplace_back(std::move(toAdd[0]));
        toAdd.clear();
    }
}

float LayeredNoiseManager::evaluate(float x, float y, float z) {
    float noise = 0.0f;
    
    for (auto& n: noiseLayers)
        noise += n->evaluate({x + offset[0], y + offset[1], z + offset[2]});
    noise *= strength;
    
    if (absv) {
        noise = abs(noise);
    }
    
    if (sq) {
        noise = noise * noise;
    }
    
    return noise;
}


}
}
