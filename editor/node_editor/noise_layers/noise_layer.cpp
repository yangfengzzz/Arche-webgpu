//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "noise_layer.h"
#include "gui/imgui.h"

#define MAKE_IMGUI_ID(x) ("##NoiseLayer" + std::to_string(index) + x).c_str()
#define MAKE_IMGUI_LABEL(x) (x + std::string("##NoiseLayer") + std::to_string(index)).c_str()

static const char *noiseTypes[] = {"Perlin", "Cellular", "OpenSimplex2", "OpenSimplex2S", "Value", "Value Cubic"};
static const char *fractalTypes[] = {"None", "FBm", "Ridged", "PingPong"};
static const char *distFuncs[] = {"EuclideanSq", "Euclidean", "Manhattan", "Euclidean Manhattan Hybrid"};

static void setNoiseType(FastNoiseLite *noiseGen, int type) {
    switch (type) {
        case 0:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
            break;

        case 1:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
            break;

        case 2:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
            break;

        case 3:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
            break;

        case 4:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Value);
            break;

        case 5:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_ValueCubic);
            break;

        default:
            noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
            break;
    }
}

static void setFractalType(FastNoiseLite *noiseGen, int type) {
    switch (type) {
        case 0:
            noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_None);
            break;

        case 1:
            noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
            break;

        case 2:
            noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_Ridged);
            break;

        case 3:
            noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_PingPong);
            break;

        default:
            noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_None);
            break;
    }
}

static void setDistanceFunc(FastNoiseLite *noiseGen, int func) {
    switch (func) {
        case 0:
            noiseGen->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq);
            break;

        case 1:
            noiseGen->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Euclidean);
            break;

        case 2:
            noiseGen->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Manhattan);
            break;

        case 3:
            noiseGen->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Hybrid);
            break;

        default:
            noiseGen->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq);
            break;
    }
}

namespace vox {
namespace editor {
NoiseLayer::NoiseLayer() {
    enabled = false;
    seed = 42;
    frequency = 0.01f;
    fractalType = 0;
    distanceFunc = 0;
    octaves = 3;
    lacunarity = 2.0f;
    gain = 0.5f;
    weightedStrength = 0.0f; // should be within 0 to 1
    pingPongStrength = 2.0f;
    strength = 1.0f;
    cellularJitter = 1.0f;
    noiseGen = std::make_unique<FastNoiseLite>();
    name.reserve(1000);
    name = "Noise Layer\0                                        ";
    offset[0] = offset[1] = offset[2] = 0.0f;
    noiseType = 0;
    noiseTypeStr = noiseTypes[0];
    fractalTypeStr = fractalTypes[0];
    distFuncStr = distFuncs[0];
}

float NoiseLayer::evaluate(NoiseLayerInput input) {
    return enabled ? noiseGen->GetNoise(input.x + offset[0], input.y + offset[1], input.z + offset[2]) * strength : 0.0f;
}

void NoiseLayer::render(int index) {
    ImGui::SameLine();
    ImGui::Text(name.c_str());
    ImGui::InputTextWithHint(MAKE_IMGUI_ID("Name Input"), "Name", name.data(), name.capacity());
    ImGui::NewLine();
    ImGui::Checkbox(MAKE_IMGUI_LABEL("Enabled"), &enabled);
    ImGui::Text("Offsets:");
    ImGui::DragFloat3(MAKE_IMGUI_ID("Offsets"), offset, 0.1f);
    ImGui::Text("Noise Type : ");
    ImGui::SameLine();

    if (ImGui::BeginCombo(MAKE_IMGUI_ID("Noise Type Selector"), noiseTypeStr)) {
        for (int n = 0; n < 6; n++) {
            bool isSelected = (noiseTypeStr == noiseTypes[n]);

            if (ImGui::Selectable(noiseTypes[n], isSelected)) {
                noiseTypeStr = noiseTypes[n];
                noiseType = n;
                setNoiseType(noiseGen.get(), noiseType);

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Text("Fractal Type : ");
    ImGui::SameLine();

    if (ImGui::BeginCombo(MAKE_IMGUI_ID("Fractal Type Selector"), fractalTypeStr)) {
        for (int n = 0; n < 4; n++) {
            bool isSelected = (fractalTypeStr == fractalTypes[n]);

            if (ImGui::Selectable(fractalTypes[n], isSelected)) {
                fractalTypeStr = fractalTypes[n];
                fractalType = n;
                setFractalType(noiseGen.get(), fractalType);

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::DragInt(MAKE_IMGUI_LABEL("Seed"), &seed)) {
        noiseGen->SetSeed(seed);
    }

    if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Frequency"), &frequency, 0.001f)) {
        noiseGen->SetFrequency(frequency);
    }

    ImGui::DragFloat(MAKE_IMGUI_LABEL("Strength"), &strength, 0.1f);

    if (fractalType != 0) {
        if (ImGui::DragInt(MAKE_IMGUI_LABEL("Octaves"), &octaves, 0.05f)) {
            noiseGen->SetFractalOctaves(octaves);
        }

        if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Lacunarity"), &lacunarity, 0.1f)) {
            noiseGen->SetFractalLacunarity(lacunarity);
        }

        if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Gain"), &gain, 0.1f)) {
            noiseGen->SetFractalGain(gain);
        }

        if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Weighted Strength"), &weightedStrength, 0.01f)) {
            noiseGen->SetFractalWeightedStrength(weightedStrength);
        }

        if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Ping Pong Strength"), &pingPongStrength, 0.01f)) {
            noiseGen->SetFractalPingPongStrength(pingPongStrength);
        }
    }

    if (noiseType == 1) {
        if (ImGui::DragFloat(MAKE_IMGUI_LABEL("Cellur Jitter"), &cellularJitter, 0.01f)) {
            noiseGen->SetCellularJitter(cellularJitter);
        }

        ImGui::Text("Cellular Distance Function : ");
        ImGui::SameLine();

        if (ImGui::BeginCombo(MAKE_IMGUI_ID("Cellular Distance Function"), distFuncStr)) {
            for (int n = 0; n < 4; n++) {
                bool isSelected = (distFuncStr == distFuncs[n]);

                if (ImGui::Selectable(distFuncs[n], isSelected)) {
                    distFuncStr = distFuncs[n];
                    distanceFunc = n;
                    setFractalType(noiseGen.get(), distanceFunc);

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

            ImGui::EndCombo();
        }
    }
}



}
}
