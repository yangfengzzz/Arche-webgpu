//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef div_node_hpp
#define div_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class DivNode : public NodeEditorNode {
public:
    DivNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float value1, value2;
};


}
}
#endif /* div_node_hpp */
