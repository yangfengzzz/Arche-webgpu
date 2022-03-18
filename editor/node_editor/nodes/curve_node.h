//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef curve_node_hpp
#define curve_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class CurveNode : public NodeEditorNode {
public:
    CurveNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();

    std::vector<ImVec2> curve;
    int maxPoints;
    int axis;
};


}
}
#endif /* curve_node_hpp */
