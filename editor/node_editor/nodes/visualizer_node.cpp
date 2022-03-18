//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "visualizer_node.h"
#include "cpu_node_editor.h"
#include "gui/implot.h"

namespace vox {
namespace editor {
VisualizerNode::VisualizerNode() {
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    map.clear();
}

NodeOutput VisualizerNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    inputC = input;

    if (inputPins[0]->isLinked()) {
        return inputPins[0]->other->evaluate(input);
    }

    return NodeOutput({0.0f});
}

void VisualizerNode::onRender() {
    drawHeader("Visualizer");
    inputPins[0]->render();
    ImGui::Text("In");
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(150, 10));
    ImGui::SameLine();
    ImGui::Text("Out");
    outputPins[0]->render();
    ImGui::NewLine();

    if (ImGui::Button("Update")) {
        if (outputPins[0]->isLinked()) {
            map.clear();
            mutex.lock();
            bool isIl = inputPins[0]->isLinked();
            NodeInputParam p;

            for (int i = 0; i < inputC.maxX; i++) {
                for (int j = 0; j < inputC.maxY; j++) {
                    if (isIl) {
                        p = inputC;
                        p.y = i;
                        p.x = j;
                        map.push_back(inputPins[0]->other->evaluate(p).value);
                    } else {
                        map.push_back(0.0f);
                    }
                }
            }

            mutex.unlock();
        }
    }

    ImGui::NewLine();
    ImGui::Text("Right Click to visualize!");

    if (map.size() > 0) {
        ImGuiNodeEditor::Suspend();

        if (ImGui::BeginPopupContextWindow(MAKE_IMGUI_LABEL(id, "Plot"))) {
            ImGui::BeginChild(MAKE_IMGUI_LABEL(id, "PlotChild"), ImVec2(300, 300));

            if (ImPlot::BeginPlot(MAKE_IMGUI_ID(id))) {
                ImPlot::PlotHeatmap<float>(MAKE_IMGUI_ID(outputPins[0]->id), map.data(), inputC.maxY, inputC.maxX);
                ImPlot::EndPlot();
            }

            ImGui::EndChild();
            ImGui::EndPopup();
        }

        ImGuiNodeEditor::Resume();
    }
}


}
}
