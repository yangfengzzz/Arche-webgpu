//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "clamp_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
ClampNode::ClampNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    minV = 0.0f;
    maxV = 1.0f;
    inpt = 0.5f;
}

void ClampNode::onRender() {
    drawHeader("Clamp");
    inputPins[0]->render();
    ImGui::Text("Input");

    if (!inputPins[0]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &inpt, 0.01f);
        ImGui::PopItemWidth();
    }

    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    inputPins[1]->render();
    ImGui::Text("Minimum");

    if (!inputPins[1]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[1]->id)).c_str(), &minV, 0.01f);
        ImGui::PopItemWidth();
    }

    inputPins[2]->render();
    ImGui::Text("Maximum");

    if (!inputPins[2]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[2]->id)).c_str(), &maxV, 0.01f);
        ImGui::PopItemWidth();
    }

    ImGui::NewLine();
}

NodeOutput ClampNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float mn, mx, vv;
    mn = 0.0f;
    mx = 1.0f;
    vv = 0.5f;

    if (inputPins[0]->isLinked()) {
        vv = inputPins[0]->other->evaluate(input).value;
    } else {
        vv = inpt;
    }

    if (inputPins[1]->isLinked()) {
        mn = inputPins[1]->other->evaluate(input).value;
    } else {
        mn = minV;
    }

    if (inputPins[2]->isLinked()) {
        mx = inputPins[2]->other->evaluate(input).value;
    } else {
        mx = maxV;
    }

    return NodeOutput({vv > mx ? mx : (vv < mn ? mn : vv)});
}

}
}
