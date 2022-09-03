//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/mesh/buffer_mesh.h"
#include "vox.render/mesh/model_mesh.h"
#include "vox.render/singleton.h"

namespace vox {
class MeshManager : public Singleton<MeshManager> {
public:
    static MeshManager &GetSingleton();

    static MeshManager *GetSingletonPtr();

    explicit MeshManager(wgpu::Device &device);

    ~MeshManager() = default;

    /**
     * @brief Loads mesh
     */
    std::shared_ptr<BufferMesh> LoadBufferMesh();

    std::shared_ptr<ModelMesh> LoadModelMesh();

    void CollectGarbage();

private:
    wgpu::Device &device_;
    std::vector<std::shared_ptr<Mesh>> mesh_pool_;
};

template <>
inline MeshManager *Singleton<MeshManager>::ms_singleton{nullptr};
}  // namespace vox
