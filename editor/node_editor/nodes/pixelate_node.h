//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pixelate_node_hpp
#define pixelate_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class PixelateNode : public NodeEditorNode {
public:
    PixelateNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float pixelSize;
};


}
}
#endif /* pixelate_node_hpp */
