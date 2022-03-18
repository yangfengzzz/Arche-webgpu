//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef visualizer_node_hpp
#define visualizer_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class VisualizerNode : public NodeEditorNode {
public:
    VisualizerNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
    
    std::vector<float> map;
    NodeInputParam inputC;
    std::mutex mutex;
};


}
}
#endif /* visualizer_node_hpp */
