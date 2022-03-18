//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "time_based_seed_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
TimeBasedSeedNode::TimeBasedSeedNode() {
    headerColor = ImColor(VALUE_NODE_COLOR);
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    val = 42;
}

NodeOutput TimeBasedSeedNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    return NodeOutput({(float) val});
}

void TimeBasedSeedNode::onRender() {
    drawHeader("Time Based Seed");
    ImGui::Dummy(ImVec2(200, 10));
    ImGui::SameLine();
    ImGui::Text("Time");
    outputPins[0]->render();
    ImGui::NewLine();
    ImGui::Text("Current Seed : ");
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGui::InputInt(MAKE_IMGUI_ID(id), &val, 1);
    ImGui::PopItemWidth();
    ImGui::NewLine();

    if (ImGui::Button("Regenerate Seed")) {
        val = static_cast<int>(time(NULL));
    }
}


}
}
