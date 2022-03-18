//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sub_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
SubNode::SubNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(MATH_NODE_COLOR);
    value1 = (0.0f);
    value2 = (0.0f);
}

NodeOutput SubNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float diff = 0;

    if (inputPins[0]->isLinked()) {
        diff += inputPins[0]->other->evaluate(input).value;
    } else {
        diff += value1;
    }

    if (inputPins[1]->isLinked()) {
        diff -= inputPins[1]->other->evaluate(input).value;
    } else {
        diff -= value2;
    }

    return NodeOutput({diff});
}

void SubNode::onRender() {
    drawHeader("Subtract");
    inputPins[0]->render();

    if (inputPins[0]->isLinked()) {
        ImGui::Text("Input 1");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &value1, 0.01f);
        ImGui::PopItemWidth();
    }

    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    inputPins[1]->render();

    if (inputPins[1]->isLinked()) {
        ImGui::Text("Input 2");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[1]->id)).c_str(), &value2, 0.01f);
        ImGui::PopItemWidth();
    }
}


}
}
