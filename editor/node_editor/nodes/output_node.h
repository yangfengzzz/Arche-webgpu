//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef output_node_hpp
#define output_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class OutputNode : public NodeEditorNode {
public:
    OutputNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    float value;
};


}
}
#endif /* output_node_hpp */
