//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_node.h"
#include "cpu_node_editor.h"

namespace vox {
namespace editor {
TextureNode::TextureNode() {
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    inputPins.push_back(new NodeEditorPin());
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    outputPins.push_back(new NodeEditorPin(NodeEditorPinType::Output));
    headerColor = ImColor(IMAGE_NODE_COLOR);
    isDefault = true;
    scale = 1.0f;
    inv = false;
    npScale = true;
    autoTiled = true;
    numTiles = 1;
    rota = 0.0f;
    posi[0] = posi[1] = 0.0f;
}

}
}
