//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/skin_mesh_renderer.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/animation/animator.h"
#include "vox.render/entity.h"

namespace vox {
bool SkinMeshRenderer::loadSkins(const char* _filename) {
    assert(_filename);
    LOGI("Loading meshes archive: {}", _filename);
    vox::io::File file(_filename, "rb");
    if (!file.opened()) {
        LOGE("Failed to open mesh file {}.", _filename)
        return false;
    }
    vox::io::IArchive archive(&file);

    while (archive.TestTag<Skin>()) {
        _skins.resize(_skins.size() + 1);
        archive >> _skins.back();
    }

    // Computes the number of skinning matrices required to skin all meshes.
    // A mesh is skinned by only a subset of joints, so the number of skinning
    // matrices might be less that the number of skeleton joints.
    // Mesh::joint_remaps is used to know how to order skinning matrices. So
    // the number of matrices required is the size of joint_remaps.
    size_t num_skinning_matrices = 0;
    for (const Skin& skin : _skins) {
        num_skinning_matrices = std::max(num_skinning_matrices, skin.joint_remaps.size());
    }
    // Allocates skinning matrices.
    _skinning_matrices.resize(num_skinning_matrices);

    return true;
}

void SkinMeshRenderer::update(float deltaTime) {
    if (!_animator) {
        _animator = entity()->getComponent<Animator>();
    }

    if (_animator && !_skins.empty()) {
        // Builds skinning matrices, based on the output of the animation stage.
        // The mesh might not use (aka be skinned by) all skeleton joints. We
        // use the joint remapping table (available from the mesh object) to
        // reorder model-space matrices and build skinning ones.
        for (const Skin& skin : _skins) {
            for (size_t i = 0; i < skin.joint_remaps.size(); ++i) {
                _skinning_matrices[i] = _animator->models()[skin.joint_remaps[i]] * skin.inverse_bind_poses[i];
            }
        }
    }
}

void SkinMeshRenderer::_updateBounds(BoundingBox3F& worldBounds) {
    if (_animator) {
        _animator->computeSkeletonBounds(worldBounds);
    }
}

}  // namespace vox