//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh_manager.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/platform/filesystem.h"

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

std::vector<std::shared_ptr<Skin>> MeshManager::LoadSkinnedMesh(const std::string &filename) {
    LOGI("Loading meshes archive: {}", filename)
    vox::io::File file((fs::path::Get(fs::path::Type::ASSETS) + filename).c_str(), "rb");
    if (!file.opened()) {
        LOGE("Failed to open mesh file {}.", filename)
    }
    vox::io::IArchive archive(&file);

    std::vector<std::shared_ptr<Skin>> result;
    while (archive.TestTag<Skin>()) {
        auto skin = std::make_shared<Skin>();
        archive >> *skin;
        skins_pool_.push_back(skin);
        result.push_back(skin);
    }
    return result;
}

void MeshManager::CollectGarbage() {
    mesh_pool_.erase(std::remove_if(mesh_pool_.begin(), mesh_pool_.end(),
                                    [](const std::shared_ptr<Mesh> &mesh) { return mesh.use_count() == 1; }),
                     mesh_pool_.end());
    skins_pool_.erase(std::remove_if(skins_pool_.begin(), skins_pool_.end(),
                                     [](const std::shared_ptr<Skin> &mesh) { return mesh.use_count() == 1; }),
                      skins_pool_.end());
}

}  // namespace vox
