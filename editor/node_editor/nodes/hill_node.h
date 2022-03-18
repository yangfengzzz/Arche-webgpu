//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef hill_node_hpp
#define hill_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class HillNode : public NodeEditorNode {
public:
    HillNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    float pos[3], height, radius, p;
};


}
}
#endif /* hill_node_hpp */
