//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mul_node_hpp
#define mul_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class MulNode : public NodeEditorNode {
public:
    MulNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float value1, value2;
};


}
}
#endif /* mul_node_hpp */
