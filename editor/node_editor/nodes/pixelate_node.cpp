//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "pixelate_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
PixelateNode::PixelateNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    pixelSize = 0.05f;
}

NodeOutput PixelateNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    if (input.maxZ == 0) {
        input.maxZ = 1;
    }

    float currX = input.x / input.maxX;
    float currY = input.y / input.maxY;
    float currZ = input.z / input.maxZ;
    float boxX = floorf(currX / pixelSize);
    float boxY = floorf(currY / pixelSize);
    float boxZ = floorf(currZ / pixelSize);
    input.x = boxX * pixelSize * input.maxX;
    input.y = boxY * pixelSize * input.maxY;
    input.z = boxZ * pixelSize * input.maxZ;

    if (inputPins[0]->isLinked())
        return NodeOutput({inputPins[0]->other->evaluate(input).value});
    return NodeOutput({0.0f});
}

void PixelateNode::onRender() {
    drawHeader("Pixelate Node");
    inputPins[0]->render();
    ImGui::Text("In");
    inputPins[1]->render();

    if (inputPins[1]->isLinked()) {
        ImGui::Text("Pixel Size");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &pixelSize, 0.01f, 0.001f, 1);
        ImGui::PopItemWidth();
    }

    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    ImGui::Text("Pixelates the input");
}



}
}
