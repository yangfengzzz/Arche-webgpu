//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "generator_mask.h"
#include "mask_layer_helper.h"
#include "gui_entry.h"

namespace vox {
namespace editor {
GeneratorMaskManager::GeneratorMaskManager(std::string uid):
uid(uid) {
}

float GeneratorMaskManager::EvaluateAt(float x, float y, float z, float value) {
    float m = 0.0f;

    for (int i = 0; i < masks.size(); i++) {
        if (masks[i].type == MASK_LAYER_HILL) {
            m += EvaluateHillMask(&masks[i], x, y, z);
        } else if (masks[i].type == MASK_LAYER_CRATOR) {
            m += EvaluateCratorMask(&masks[i], x, y, z);
        } else if (masks[i].type == MASK_LAYER_CLIFF) {
            m += EvaluateCliffMask(&masks[i], x, y, z);
        }
    }

    switch (type) {
        case GeneratorMask_Additive: {
            value = value + m;
            break;
        }

        case GeneratorMask_AverageAdditive: {
            m = m / (float) masks.size();
            value = value + m;
            break;
        }

        case GeneratorMask_Multiplicative: {
            value = value * m;
            break;
        }

        case GeneratorMask_AverageMultiplicative: {
            m = m / (float) masks.size();
            value = value * m;
            break;
        }

        default:
            break;
    };

    return value;
}

void GeneratorMaskManager::ShowSettings() {
    ImGui::Checkbox(("Enabled##GMSK" + uid).c_str(), &enabled);

    if (ImGui::BeginCombo("Masking Mode##GMSK", generator_mask_type_names[type])) {
        for (int n = 0; n < IM_ARRAYSIZE(generator_mask_type_names); n++) {
            bool is_selected = (type == n);

            if (ImGui::Selectable(generator_mask_type_names[n], is_selected)) {
                type = (GeneratorMaskType) n;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();    // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }

        ImGui::EndCombo();
    }

    for (int i = 0; i < masks.size(); i++) {
        if (ImGui::CollapsingHeader(("Custom Base Mask Layer " + std::to_string(i) + "##GMSK" + uid).c_str())) {
            if (masks[i].type == MASK_LAYER_HILL) {
                ShowHillMaskSettingS(&masks[i], uid + std::to_string(i));
            } else if (masks[i].type == MASK_LAYER_CRATOR) {
                ShowCratorMaskSettingS(&masks[i], uid + std::to_string(i));
            } else if (masks[i].type == MASK_LAYER_CLIFF) {
                ShowCliffMaskSettingS(&masks[i], uid + std::to_string(i));
            }

            if (ImGui::Button(("Delete##GMSK" + std::to_string(i) + uid).c_str())) {
                while (GUIEntry::getSingleton().states.remeshing);

                masks.erase(masks.begin() + i);
                break;
            }
        }

        ImGui::Separator();
    }

    if (ImGui::Button(("Add##GMSK" + uid).c_str())) {
        ImGui::OpenPopup(("AddMaskLayer##GMSK" + uid).c_str());
    }

    if (ImGui::BeginPopup(("AddMaskLayer##GMSK" + uid).c_str())) {
        if (ImGui::Button(("Hill##GMSK" + uid).c_str())) {
            masks.push_back(GeneratorMask());
            masks.back().type = MASK_LAYER_HILL;
            masks.back().d1[0] = masks.back().d1[1] = masks.back().d1[2] = 1.0f;
            gmcount++;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button(("Crater##GMSK" + uid).c_str())) {
            masks.push_back(GeneratorMask());
            masks.back().type = MASK_LAYER_CRATOR;
            masks.back().d1[0] = masks.back().d3[0] = masks.back().d3[1] = 1.0f;
            gmcount++;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button(("Cliff##GMSK" + uid).c_str())) {
            masks.push_back(GeneratorMask());
            masks.back().type = MASK_LAYER_CLIFF;
            masks.back().d1[2] = masks.back().d1[1] = 1.0f;
            gmcount++;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}



}
}
