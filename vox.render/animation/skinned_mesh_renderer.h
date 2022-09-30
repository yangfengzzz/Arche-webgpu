//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/skin.h"
#include "vox.render/mesh/mesh_renderer.h"

namespace vox {
class Animator;

class SkinnedMeshRenderer : public MeshRenderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit SkinnedMeshRenderer(Entity *entity);

    // Loads n Skins from an ozz archive file named _filename.
    // This function will fail and return false if the file cannot be opened or if
    // it is not a valid ozz mesh archive. A valid mesh archive can be
    // produced with ozz tools (fbx2skin) or using ozz animation serialization API.
    // _filename and _mesh must be non-nullptr.
    bool loadSkins(const std::string &filename);

    void update(float deltaTime) override;

    void render(std::vector<RenderElement> &opaqueQueue,
                std::vector<RenderElement> &alphaTestQueue,
                std::vector<RenderElement> &transparentQueue) override;

private:
    void _updateBounds(BoundingBox3F &worldBounds) override;

    void _createMesh(const Skin &skin);

private:
    vox::vector<ModelMeshPtr> _meshes;
    vox::vector<Skin> _skins;
    // Buffer of skinning matrices, result of the joint multiplication of the
    // inverse bind pose with the model space matrix.
    vox::vector<vox::simd_math::Float4x4> _skinning_matrices;
    const std::string _skinningMatrixProperty;

    Animator *_animator{nullptr};
};
}  // namespace vox