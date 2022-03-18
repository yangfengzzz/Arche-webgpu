//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef texture_coordinates_node_hpp
#define texture_coordinates_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class TextureCoordinatesNode : public NodeEditorNode {
public:
    TextureCoordinatesNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
};


}
}
#endif /* texture_coordinates_node_hpp */
