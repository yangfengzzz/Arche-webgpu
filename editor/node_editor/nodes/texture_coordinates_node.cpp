//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_coordinates_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
TextureCoordinatesNode::TextureCoordinatesNode() {
    headerColor = ImColor(VALUE_NODE_COLOR);
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
}

NodeOutput TextureCoordinatesNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    int id = pin->id;

    if (outputPins[0]->id == id)
        return NodeOutput({input.texX});
    else if (outputPins[1]->id == id)
        return NodeOutput({input.texY});
    return NodeOutput();
}

void TextureCoordinatesNode::onRender() {
    drawHeader("Texture Coordinates");
    ImGui::Dummy(ImVec2(150, 10));
    ImGui::SameLine();
    ImGui::Text("X");
    outputPins[0]->render();
    ImGui::Dummy(ImVec2(150, 10));
    ImGui::SameLine();
    ImGui::Text("Y");
    outputPins[1]->render();
}


}
}
