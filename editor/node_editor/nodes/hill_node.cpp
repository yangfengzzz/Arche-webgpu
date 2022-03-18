//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hill_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
#define SQUARE(x) (x) * (x)

HillNode::HillNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(MATH_NODE_COLOR);
    pos[0] = pos[1] = pos[2] = 0;
    height = 1.0f;
    radius = 1.0f;
    p = 1.0f;
}

void HillNode::onRender() {
    drawHeader("Hill");
    ImGui::Dummy(ImVec2(150, 20));
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    ImGui::PushItemWidth(150);
    ImGui::DragFloat2(MAKE_IMGUI_LABEL(inputPins[0]->id, "Position"), pos, 0.01f);
    ImGui::DragFloat(MAKE_IMGUI_LABEL(inputPins[1]->id, "Height"), &height, 0.01f);
    ImGui::DragFloat(MAKE_IMGUI_LABEL(inputPins[2]->id, "Radius"), &radius, 0.01f, 0.00001f);
    ImGui::DragFloat(MAKE_IMGUI_LABEL(inputPins[3]->id, "Plane Factor"), &p, 0.01f, 0.00001f);
    ImGui::PopItemWidth();
}

NodeOutput HillNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float xC, yC, hC;

    if (inputPins[0]->isLinked()) {
        xC = inputPins[0]->other->evaluate(input).value;
    } else {
        xC = pos[0];
    }

    if (inputPins[1]->isLinked()) {
        yC = inputPins[1]->other->evaluate(input).value;
    } else {
        yC = pos[1];
    }

    if (inputPins[2]->isLinked()) {
        hC = inputPins[2]->other->evaluate(input).value;
    } else {
        hC = height;
    }

    float xN = input.x / input.maxX;
    float yN = input.z / input.maxZ; // TEMP
    xN = xN * 2 - 1;
    yN = yN * 2 - 1;
    float h = 1 - (SQUARE((yN - yC) / radius) + SQUARE((xN - xC) / radius));
    h *= pow(2.71828, -p * (SQUARE(xN - xC) + SQUARE(yN - yC)));
    return NodeOutput({h * height});
}

}
}
