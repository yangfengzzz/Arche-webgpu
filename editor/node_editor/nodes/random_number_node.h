//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef random_number_node_hpp
#define random_number_node_hpp

#include "../node_editor.h"
#include <random>

namespace vox {
namespace editor {
class RandomNumberNode : public NodeEditorNode {
public:
    RandomNumberNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    int seed, min, max;
    std::mt19937 engine;
};


}
}
#endif /* random_number_node_hpp */
