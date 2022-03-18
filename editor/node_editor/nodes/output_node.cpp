//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "output_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
OutputNode::OutputNode() {
    name = "Output";
    value = 0;
    headerColor = ImColor(OUTPUT_NODE_COLOR);
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
}

void OutputNode::onRender() {
    drawHeader("Output");
    inputPins[0]->render();
    
    if (!inputPins[0]->isLinked()) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &value, 0.01f);
        ImGui::PopItemWidth();
    } else {
        ImGui::Dummy(ImVec2(100, 40));
    }
}

NodeOutput OutputNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    if (inputPins[0]->isLinked())
        return NodeOutput({inputPins[0]->other->evaluate(input)});
    return NodeOutput({value});
}

}
}
