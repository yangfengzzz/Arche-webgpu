//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "noise_cellular_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
static const char *fractalTypes[] = {"None", "FBm", "Ridged", "PingPong"};
static const char *distFuncs[] = {"EuclideanSq", "Euclidean", "Manhattan", "Euclidean Manhattan Hybrid"};

NoiseCellularNode::NoiseCellularNode() {
    headerColor = ImColor(NOISE_NODE_COLOR);
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
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
    noiseGen->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
}

NodeOutput NoiseCellularNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    if (!inputPins[0]->isLinked()) {
        noiseGen->SetSeed(seed);
    } else {
        noiseGen->SetSeed((int) inputPins[0]->other->evaluate(input).value);
    }
    
    if (!inputPins[2]->isLinked()) {
        noiseGen->SetFrequency(frequency);
    } else {
        noiseGen->SetFrequency(inputPins[2]->other->evaluate(input).value);
    }
    
    if (fractalType == 0) {
        noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_None);
    } else if (fractalType == 1) {
        noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    } else if (fractalType == 2) {
        noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_Ridged);
    } else if (fractalType == 3) {
        noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_PingPong);
    } else {
        noiseGen->SetFractalType(FastNoiseLite::FractalType::FractalType_None);
    }
    
    if (!inputPins[1]->isLinked()) {
        noiseGen->SetFractalOctaves(octaves);
    } else {
        noiseGen->SetFrequency(inputPins[1]->other->evaluate(input).value);
    }
    
    if (!inputPins[3]->isLinked()) {
        noiseGen->SetFractalLacunarity(lacunarity);
    } else {
        noiseGen->SetFractalLacunarity(inputPins[3]->other->evaluate(input).value);
    }
    
    if (!inputPins[4]->isLinked()) {
        noiseGen->SetFractalGain(gain);
    } else {
        noiseGen->SetFractalGain(inputPins[4]->other->evaluate(input).value);
    }
    
    if (!inputPins[5]->isLinked()) {
        noiseGen->SetFractalWeightedStrength(weightedStrength);
    } else {
        noiseGen->SetFractalWeightedStrength(inputPins[5]->other->evaluate(input).value);
    }
    
    if (!inputPins[6]->isLinked()) {
        noiseGen->SetFractalPingPongStrength(pingPongStrength);
    } else {
        noiseGen->SetFractalPingPongStrength(inputPins[6]->other->evaluate(input).value);
    }
    
    if (!inputPins[8]->isLinked()) {
        noiseGen->SetCellularJitter(cellularJitter);
    } else {
        noiseGen->SetCellularJitter(inputPins[8]->other->evaluate(input).value);
    }
    
    float st = strength;
    
    if (inputPins[7]->isLinked()) {
        st = inputPins[7]->other->evaluate(input).value;
    }
    
    return NodeOutput({noiseGen->GetNoise(input.x, input.y, input.z) * st});
}

void NoiseCellularNode::onRender() {
    drawHeader("Cellular Noise");
    ImGui::Dummy(ImVec2(150, 10));
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    inputPins[0]->render();
    ImGui::Text("Seed");
    
    if (!inputPins[0]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragInt(MAKE_IMGUI_ID(inputPins[0]->id), &seed, 1);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[1]->render();
    ImGui::Text("Octaves");
    
    if (!inputPins[1]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragInt(MAKE_IMGUI_ID(inputPins[1]->id), &octaves, 1);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[2]->render();
    ImGui::Text("Frequency");
    
    if (!inputPins[2]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[2]->id), &frequency, 0.001f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[3]->render();
    ImGui::Text("Lacunarity");
    
    if (!inputPins[3]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[3]->id), &lacunarity, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[4]->render();
    ImGui::Text("Gain");
    
    if (!inputPins[4]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[4]->id), &gain, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[5]->render();
    ImGui::Text("Weighted Strength");
    
    if (!inputPins[5]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[5]->id), &weightedStrength, 0.01f, 0, 1);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[6]->render();
    ImGui::Text("Ping Pong Strength");
    
    if (!inputPins[6]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[6]->id), &pingPongStrength, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[8]->render();
    ImGui::Text("Cellular Jitter");
    
    if (!inputPins[8]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[8]->id), &cellularJitter, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    inputPins[7]->render();
    ImGui::Text("Strength");
    
    if (!inputPins[7]->isLinked()) {
        ImGui::Dummy(ImVec2(30, 10));
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(MAKE_IMGUI_ID(inputPins[7]->id), &strength, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::NewLine();
    }
    
    ImGui::NewLine();
    ImGui::Text("Current Fractal Type : ");
    ImGui::SameLine();
    ImGui::Text(fractalTypes[fractalType]);
    
    if (ImGui::Button(MAKE_IMGUI_LABEL(id, "Change Fractal Type"))) {
        fractalType++;
        
        if (fractalType == 4) {
            fractalType = 0;
        }
    }
    
    ImGui::NewLine();
    ImGui::Text("Current Distance Function : ");
    ImGui::SameLine();
    ImGui::Text(distFuncs[distanceFunc]);
    
    if (ImGui::Button(MAKE_IMGUI_LABEL(id, "Change Distance Function"))) {
        distanceFunc++;
        
        if (distanceFunc == 4) {
            distanceFunc = 0;
        }
    }
}


}
}
