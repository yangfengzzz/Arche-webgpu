//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/script.h"
#include "vox.toolkit/skeleton_view/skeleton_material.h"

namespace vox {
class Animator;
class SkeletonView : public Script {
public:
    explicit SkeletonView(Entity *entity);

    void onAwake() override;

    void onUpdate(float deltaTime) override;

private:
    void _createBoneMesh();

    void _createJointMesh();

    float kInter = .2f;
    Animator* _animator{nullptr};
    std::vector<float> _skeletonBuffer;

    ModelMeshPtr _boneMesh{nullptr};
    std::shared_ptr<BoneMaterial> _boneMaterial{nullptr};

    ModelMeshPtr _jointMesh{nullptr};
    std::shared_ptr<JointMaterial> _jointMaterial{nullptr};
};
}  // namespace vox