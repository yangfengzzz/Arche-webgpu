//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/grid/grid_control.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"

namespace vox::grid {
ModelMeshPtr GridControl::createGridPlane() {
    auto mesh = MeshManager::GetSingleton().LoadModelMesh();

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
    mesh->addSubMesh(0, 6, wgpu::PrimitiveTopology::TriangleList);
    return mesh;
}

GridControl::GridControl(Entity* entity) : Script(entity) {}

std::shared_ptr<GridMaterial> GridControl::getMaterial() { return _material; }

bool GridControl::is2DGrid() const { return _is2DGrid; }

void GridControl::setIs2DGrid(bool value) {
    _is2DGrid = value;
    _progress = 0;
    _flipGrid = true;
}

void GridControl::onAwake() {
    auto gridRenderer = entity()->addComponent<MeshRenderer>();
    gridRenderer->setMesh(GridControl::createGridPlane());
    _material = std::make_shared<GridMaterial>(scene()->device());
    gridRenderer->setMaterial(_material);
}

void GridControl::onUpdate(float deltaTime) {
    if (camera == nullptr) return;

    _material->setNearClipPlane(camera->nearClipPlane());
    _material->setFarClipPlane(camera->farClipPlane());

    if (_flipGrid) {
        _progress += deltaTime / 1000;
        float percent = clamp(_progress * speed, 0.f, 1.f);
        if (percent >= 1) {
            _flipGrid = false;
        }

        if (!_is2DGrid) {
            percent = 1 - percent;
        }
        _material->setFlipProgress(percent);
    }
}

}  // namespace vox::grid