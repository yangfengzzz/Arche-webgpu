//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/material.h"
#include "vox.render/script.h"

namespace vox {
class Animator;

namespace skeleton_view {
class SkeletonView : public Script {
public:
    explicit SkeletonView(Entity* entity);

    void onAwake() override;

    void onUpdate(float deltaTime) override;

private:
    void _createBoneMesh();

    void _createJointMesh();

    void _createMeshLayout();

    float kInter = .2f;
    Animator* _animator{nullptr};
    std::vector<float> _skeletonData;
    Buffer _skeletonBuffer;
    std::vector<Buffer> _boneBufferBindings;
    std::vector<Buffer> _jointBufferBindings;

    std::vector<wgpu::VertexAttribute> _vertex_attributes;
    std::vector<wgpu::VertexAttribute> _instance_attributes;
    std::vector<wgpu::VertexBufferLayout> _layouts;

    std::shared_ptr<BufferMesh> _boneMesh{nullptr};
    std::shared_ptr<Material> _boneMaterial{nullptr};

    std::shared_ptr<BufferMesh> _jointMesh{nullptr};
    std::shared_ptr<Material> _jointMaterial{nullptr};
};

}  // namespace skeleton_view
}  // namespace vox