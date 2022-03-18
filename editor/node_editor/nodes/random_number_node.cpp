//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "random_number_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
RandomNumberNode::RandomNumberNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(NOISE_NODE_COLOR);
    seed = 42;
    min = 0;
    max = 10;
}

NodeOutput RandomNumberNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    int s, mx, mn;
    s = mx = mn = 0;
    
    if (inputPins[0]->isLinked()) {
        s = inputPins[0]->other->evaluate(input).value;
    } else {
        s = seed;
    }
    
    if (inputPins[1]->isLinked()) {
        mn *= inputPins[1]->other->evaluate(input).value;
    } else {
        mn *= min;
    }
    
    if (inputPins[2]->isLinked()) {
        mx *= inputPins[2]->other->evaluate(input).value;
    } else {
        mx *= max;
    }
    
    if (mn > mx) {
        int t = mn;
        mn = mx;
        mx = t;
    }
    
    std::seed_seq seq{seed};
    engine.seed(seq);
    std::uniform_int_distribution<int> dist(mn, mx);
    return NodeOutput({(float) dist(engine)});
}

void RandomNumberNode::onRender() {
    drawHeader("Random Number");
    inputPins[0]->render();
    ImGui::Text("Seed");
    
    if (!inputPins[0]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragInt(("##" + std::to_string(inputPins[0]->id)).c_str(), &seed, 0.1f);
        ImGui::PopItemWidth();
    }
    
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    inputPins[1]->render();
    ImGui::Text("Minimum");
    
    if (!inputPins[1]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragInt(("##" + std::to_string(inputPins[1]->id)).c_str(), &min, 0.1f);
        ImGui::PopItemWidth();
    }
    
    inputPins[2]->render();
    ImGui::Text("Maximum");
    
    if (!inputPins[2]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragInt(("##" + std::to_string(inputPins[2]->id)).c_str(), &max, 0.1f);
        ImGui::PopItemWidth();
    }
}


}
}
