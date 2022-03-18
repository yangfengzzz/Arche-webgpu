//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef min_mesh_coordinates_node_hpp
#define min_mesh_coordinates_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class MinMeshCoordinatesNode : public NodeEditorNode {
public:
    MinMeshCoordinatesNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
};


}
}
#endif /* min_mesh_coordinates_node_hpp */
