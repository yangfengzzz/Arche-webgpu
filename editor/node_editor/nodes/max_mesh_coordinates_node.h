//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef max_mesh_coordinates_node_hpp
#define max_mesh_coordinates_node_hpp

#include "../node_editor.h"

namespace vox {
namespace editor {
class MaxMeshCoordinatesNode : public NodeEditorNode {
public:
    MaxMeshCoordinatesNode();

    virtual NodeOutput evaluate(NodeInputParam input, NodeEditorPin *pin);

    virtual void onRender();
};


}
}
#endif /* max_mesh_coordinates_node_hpp */
