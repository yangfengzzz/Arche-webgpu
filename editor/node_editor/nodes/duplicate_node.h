//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef duplicate_node_hpp
#define duplicate_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class DuplicateNode : public NodeEditorNode {
public:
    DuplicateNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
};


}
}
#endif /* duplicate_node_hpp */
