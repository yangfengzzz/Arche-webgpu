//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "curve_node.h"
#include "cpu_node_editor.h"
#include "imgui_curve.h"

namespace vox {
namespace editor {
// Temporary

template<typename t>
static void ReserveVector(std::vector<t> &vec, int amount) {
    size_t size = vec.size();

    for (size_t i = size; i <= amount; i++) {
        vec.push_back(t());
    }
}

static const char *axises[3] = {"X", "Y", "Z"};

CurveNode::CurveNode() {
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    maxPoints = 10;
    ReserveVector(curve, maxPoints);
    curve[0].x = -1;
    axis = 0;
}

void CurveNode::onRender() {
    drawHeader("Curve Editor");
    ImGui::Dummy(ImVec2(150, 10));
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    ImGui::Text("Max Points");
    ImGui::PushItemWidth(100);

    if (ImGui::DragInt(("##dI" + std::to_string(id)).c_str(), &maxPoints, 1, 10, 256)) {
        ReserveVector(curve, maxPoints);
    }

    ImGui::PopItemWidth();
    ImGui::NewLine();

    if (ImGui::Curve(("##" + std::to_string(id)).c_str(), ImVec2(200, 200), maxPoints, curve.data())) {
    }

    ImGui::Text("Current Axis: ");
    ImGui::SameLine();
    ImGui::Text(axises[axis]);

    if (ImGui::Button(("Change Axis##" + std::to_string(id)).c_str())) {
        axis++;

        if (axis == 3) {
            axis = 0;
        }
    }
}

NodeOutput CurveNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float axisVal = 0;
    float axisMax = 0;

    switch (axis) {
        case 0 :
            axisVal = input.x;
            axisMax = input.maxX;
            break;

        case 1:
            axisVal = input.y;
            axisMax = input.maxY;
            break;

        case 2:
            axisVal = input.z;
            axisMax = input.maxZ;
            break;

        default:
            break;
    }

    if (axisMax == 0) {
        axisMax = 1;
    }

    return NodeOutput({ImGui::CurveValueSmooth((axisVal / axisMax), maxPoints, curve.data())});
}

}
}
