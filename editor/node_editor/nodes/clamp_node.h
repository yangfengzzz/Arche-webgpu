//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef clamp_node_hpp
#define clamp_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class ClampNode : public NodeEditorNode {
public:
    ClampNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float minV, maxV, inpt;
};


}
}
#endif /* clamp_node_hpp */
