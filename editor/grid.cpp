//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "grid.h"
#include "entity.h"
#include "scene.h"
#include "mesh_renderer.h"
#include "mesh/model_mesh.h"
#include "material/base_material.h"

namespace vox {
namespace editor {
class GridMaterial : public BaseMaterial {
public:
    GridMaterial() : BaseMaterial(Shader::find("editor-grid")) {
        setIsTransparent(true);
    }
};

Grid::Grid(Entity *entity) :
Script(entity) {
    Shader::create("editor-grid", "vertex_grid", "fragment_grid");
    
    _renderer = entity->addComponent<MeshRenderer>();
    _renderer->setMesh(createPlane(entity->scene()->device()));
    _renderer->setMaterial(std::make_shared<GridMaterial>());
}

ModelMeshPtr Grid::createPlane(MTL::Device *device) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    auto positions = std::vector<Vector3F>(4);
    positions[0] = Vector3F(-1, -1, 0);
    positions[1] = Vector3F(1, -1, 0);
    positions[2] = Vector3F(-1, 1, 0);
    positions[3] = Vector3F(1, 1, 0);
    
    auto indices = std::vector<uint32_t>(6);
    indices[0] = 1;
    indices[1] = 2;
    indices[2] = 0;
    indices[3] = 1;
    indices[4] = 3;
    indices[5] = 2;
    
    mesh->setPositions(positions);
    mesh->setIndices(indices);
    mesh->uploadData(true);
    
    return mesh;
}

}
}
