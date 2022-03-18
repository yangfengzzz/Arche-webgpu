//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cos_node_hpp
#define cos_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class CosNode : public NodeEditorNode {
public:
    CosNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float value1, value2;
};


}
}
#endif /* cos_node_hpp */
