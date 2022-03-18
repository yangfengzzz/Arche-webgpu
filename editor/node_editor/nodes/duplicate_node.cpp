//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "duplicate_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
DuplicateNode::DuplicateNode() {
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
}

void DuplicateNode::onRender() {
    drawHeader("Duplicate");
    inputPins[0]->render();
    ImGui::Text("Value");
    ImGui::Dummy(ImVec2(150, 20));
    ImGui::SameLine();
    ImGui::Text("Out 1");
    outputPins[0]->render();
    ImGui::Dummy(ImVec2(150, 20));
    ImGui::SameLine();
    ImGui::Text("Out 2");
    outputPins[1]->render();
    ImGui::Dummy(ImVec2(150, 20));
    ImGui::SameLine();
    ImGui::Text("Out 3");
    outputPins[2]->render();
    ImGui::Dummy(ImVec2(150, 20));
    ImGui::SameLine();
    ImGui::Text("Out 4");
    outputPins[3]->render();
}

NodeOutput DuplicateNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    if (inputPins[0]->isLinked())
        return NodeOutput({inputPins[0]->other->evaluate(input)});
    return NodeOutput({0.0f});
}

}
}
