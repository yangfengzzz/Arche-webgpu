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

    void setSkinnedMesh(const std::shared_ptr<Skin> &skin);

    void update(float deltaTime) override;

private:
    void _updateBounds(BoundingBox3F &worldBounds) override;

    void _createMesh();

private:
    std::shared_ptr<Skin> _skin{nullptr};
    // Buffer of skinning matrices, result of the joint multiplication of the
    // inverse bind pose with the model space matrix.
    std::vector<vox::simd_math::Float4x4> _skinning_matrices;
    static const std::string _skinningMatrixProperty;

    Animator *_animator{nullptr};
};
}  // namespace vox