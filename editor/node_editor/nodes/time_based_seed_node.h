//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef time_based_seed_node_hpp
#define time_based_seed_node_hpp

#include "../node_editor.h"
#include <random>

namespace vox {
namespace editor {
class TimeBasedSeedNode : public NodeEditorNode {
public:
    TimeBasedSeedNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    int val;
};


}
}
#endif /* time_based_seed_node_hpp */
