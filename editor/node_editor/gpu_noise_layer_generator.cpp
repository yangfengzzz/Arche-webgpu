//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gpu_noise_layer_generator.h"
#include "gui_entry.h"

namespace vox {
namespace editor {
void GPUNoiseLayerGenerator::showSetting(int id) {
    ImGui::Checkbox(("Enabled##GPUNL" + std::to_string(id)).c_str(), &enabled);
    ImGui::DragInt(("Local Work Group Size##GPUNL" + std::to_string(id)).c_str(), &localSize);

    if (ImGui::Button(("Edit##GPUNL" + std::to_string(id)).c_str())) {
        windowStat = true;
    }

    ImGui::Text("Vertices Processed : %d", vc);
    ImGui::Text("Time : %lf ms", time);
}

void GPUNoiseLayerGenerator::update() {
    if (windowStat) {
        ImGui::Begin((name + "##" + uid).c_str(), &windowStat);
        ImGui::Text("Global Settings");
        ImGui::DragFloat(("Offset X" + std::string("##GPUNL")).c_str(), &noiseLayers[0].offsetX, 0.01f);
        ImGui::DragFloat(("Offset Y" + std::string("##GPUNL")).c_str(), &noiseLayers[0].offsetY, 0.01f);
        ImGui::DragFloat(("Offset Z" + std::string("##GPUNL")).c_str(), &noiseLayers[0].offsetZ, 0.01f);
        ImGui::DragFloat(("Strength" + std::string("##GPUNL")).c_str(), &noiseLayers[0].strength, 0.01f);
        ImGui::DragFloat(("Frequency" + std::string("##GPUNL")).c_str(), &noiseLayers[0].frequency, 0.001f);
        ImGui::Separator();

        for (int i = 1; i < noiseLayers.size(); i++) {
            if (ImGui::CollapsingHeader(("Noise Layer " + std::to_string(i + 1)).c_str())) {
                ImGui::Text("Noise Layer %d", (i + 1));
                ImGui::DragFloat(("Offset X" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].offsetX, 0.01f);
                ImGui::DragFloat(("Offset Y" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].offsetY, 0.01f);
                ImGui::DragFloat(("Offset Z" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].offsetZ, 0.01f);
                ImGui::DragFloat(("Strength" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].strength, 0.01f);
                ImGui::DragFloat(("Frequency" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].frequency, 0.001f);
                ImGui::DragFloat(("Domain Wrap Depth" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].domainWrapDepth, 0.1f);

                if (noiseLayers[i].fractal != 0) {
                    ImGui::DragFloat(("Octaves" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].octaves, 0.01f, 1, 128);
                    ImGui::DragFloat(("Lacunarity" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].lacunarity, 0.01f);
                    ImGui::DragFloat(("Gain" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].gain, 0.01f);
                    ImGui::DragFloat(("Weighted Strength" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].weightedStrength, 0.01f);
                    ImGui::DragFloat(("Ping Pong Strength" + std::string("##GPUNL") + std::to_string(i)).c_str(), &noiseLayers[i].pingPongStrength, 0.01f);
                }

                ImGui::Text("Fractal Type %f", noiseLayers[i].fractal);

                if (ImGui::Button(("Change Fractal Type##GPUNL" + std::to_string(i)).c_str())) {
                    noiseLayers[i].fractal += 1;

                    if (noiseLayers[i].fractal == 4) {
                        noiseLayers[i].fractal = 0.0f;
                    }
                }
            }

            if (ImGui::Button(("Delete" + std::string("##GPUNL") + std::to_string(i)).c_str())) {
                while (GUIEntry::getSingleton().states.remeshing);

                noiseLayers.erase(noiseLayers.begin() + i);
                break;
            }

            ImGui::Separator();
        }

        if (ImGui::Button("Add##GPUNL")) {
            noiseLayers.push_back(GPUNoiseLayer());
        }

        ImGui::End();
    }
}


}
}
