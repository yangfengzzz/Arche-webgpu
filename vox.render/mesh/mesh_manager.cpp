//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh_manager.h"

namespace vox {
MeshManager::MeshManager(wgpu::Device &device) : device_(device) {}

MeshManager *MeshManager::GetSingletonPtr() { return ms_singleton; }

MeshManager &MeshManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

std::shared_ptr<BufferMesh> MeshManager::LoadBufferMesh() {
    auto mesh = std::make_shared<BufferMesh>();
    mesh_pool_.emplace_back(mesh);
    return mesh;
}

std::shared_ptr<ModelMesh> MeshManager::LoadModelMesh() {
    auto mesh = std::make_shared<ModelMesh>(device_);
    mesh_pool_.emplace_back(mesh);
    return mesh;
}

void MeshManager::CollectGarbage() {
    mesh_pool_.erase(std::remove_if(mesh_pool_.begin(), mesh_pool_.end(),
                                    [](const std::shared_ptr<Mesh> &mesh) { return mesh.use_count() == 1; }),
                     mesh_pool_.end());
}

}  // namespace vox
