//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sin_node_hpp
#define sin_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class SinNode : public NodeEditorNode {
public:
    SinNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float value1, value2;
};


}
}
#endif /* sin_node_hpp */
