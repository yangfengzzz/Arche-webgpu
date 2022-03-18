//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blend_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
BlendNode::BlendNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    value1 = 0.0f;
    value2 = 1.0f;
    factor = 0.5f;
}

void BlendNode::onRender() {
    drawHeader("Blend");
    inputPins[0]->render();
    ImGui::Text("Value A");

    if (!inputPins[0]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &value1, 0.01f);
        ImGui::PopItemWidth();
    }

    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    inputPins[1]->render();
    ImGui::Text("Value B");

    if (!inputPins[1]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[1]->id)).c_str(), &value2, 0.01f);
        ImGui::PopItemWidth();
    }

    inputPins[2]->render();
    ImGui::Text("Factor");

    if (!inputPins[2]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[2]->id)).c_str(), &factor, 0.01f, 0, 1);
        ImGui::PopItemWidth();
    }

    ImGui::NewLine();
    ImGui::Text("Blend Mode");
}

NodeOutput BlendNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float v1, v2, f;
    v1 = v2 = f = 0;

    if (inputPins[0]->isLinked()) {
        v1 = inputPins[0]->other->evaluate(input).value;
    } else {
        v1 = value1;
    }

    if (inputPins[1]->isLinked()) {
        v2 = inputPins[1]->other->evaluate(input).value;
    } else {
        v2 = value2;
    }

    if (inputPins[2]->isLinked()) {
        f = inputPins[2]->other->evaluate(input).value;
    } else {
        f = factor;
    }

    return NodeOutput({f * v1 + (1 - f) * v2});
}


}
}
