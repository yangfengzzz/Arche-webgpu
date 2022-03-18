//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "min_val_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
MinValNode::MinValNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin()); // FOR FUTURE
    inputPins.push_back(new NodeEditorPin()); // FOR FUTURE
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(OP_NODE_COLOR);
    outputf = inputf = outputr = thresholdf = 0.0f;
    thresholdf = 0.5f;
}

void MinValNode::onRender() {
    drawHeader("Min Val");
    inputPins[0]->render();
    
    if (inputPins[0]->isLinked()) {
        ImGui::Text("Input");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[0]->id)).c_str(), &inputf, 0.01f);
        ImGui::PopItemWidth();
    }
    
    ImGui::SameLine();
    ImGui::Text("Output");
    outputPins[0]->render();
    inputPins[1]->render();
    
    if (inputPins[1]->isLinked()) {
        ImGui::Text("Threshold");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[1]->id)).c_str(), &thresholdf, 0.01f);
        ImGui::PopItemWidth();
    }
    
    inputPins[2]->render();
    
    if (inputPins[2]->isLinked()) {
        ImGui::Text("Output F");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[2]->id)).c_str(), &outputf, 0.01f);
        ImGui::PopItemWidth();
    }
    
    inputPins[3]->render();
    
    if (inputPins[3]->isLinked()) {
        ImGui::Text("Output R");
    } else {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat(("##" + std::to_string(inputPins[3]->id)).c_str(), &outputr, 0.01f);
        ImGui::PopItemWidth();
    }
}

NodeOutput MinValNode::evaluate(NodeInputParam input, NodeEditorPin *pin) {
    float x = inputf;
    
    if (inputPins[0]->isLinked()) {
        x = inputPins[0]->other->evaluate(input).value;
    }
    
    float t = thresholdf;
    
    if (inputPins[1]->isLinked()) {
        t = inputPins[1]->other->evaluate(input).value;
    }
    
    if (x > t) {
        if (inputPins[3]->isLinked())
            return NodeOutput({inputPins[3]->evaluate(input)});
        else
            return NodeOutput({outputr});
    } else {
        if (inputPins[2]->isLinked())
            return NodeOutput({inputPins[2]->evaluate(input)});
        else
            return NodeOutput({outputf});
    }
}


}
}
