//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/skeleton_view/skeleton_view.h"

#include "vox.animation/runtime/skeleton_utils.h"
#include "vox.render/animation/animator.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"

namespace vox {
namespace {
int fillPostureUniforms(const animation::Skeleton& _skeleton,
                        const span<const simd_math::Float4x4>& _matrices,
                        float* _uniforms) {
    assert(IsAligned(_uniforms, alignof(math::SimdFloat4)));

    // Prepares computation constants.
    const int num_joints = _skeleton.num_joints();
    const span<const int16_t>& parents = _skeleton.joint_parents();

    int instances = 0;
    for (int i = 0; i < num_joints && instances < animation::Skeleton::kMaxJoints * 2; ++i) {
        // Root isn't rendered.
        const int16_t parent_id = parents[i];
        if (parent_id == animation::Skeleton::kNoParent) {
            continue;
        }

        // Selects joint matrices.
        const simd_math::Float4x4& parent = _matrices[parent_id];
        const simd_math::Float4x4& current = _matrices[i];

        // Copy parent joint's raw matrix, to render a bone between the parent
        // and current matrix.
        float* uniform = _uniforms + instances * 16;
        std::memcpy(uniform, parent.cols, 16 * sizeof(float));

        // Set bone direction (bone_dir). The shader expects to find it at index
        // [3,7,11] of the matrix.
        // Index 15 is used to store whether a bone should be rendered,
        // otherwise it's a leaf.
        float bone_dir[4];
        simd_math::StorePtrU(current.cols[3] - parent.cols[3], bone_dir);
        uniform[3] = bone_dir[0];
        uniform[7] = bone_dir[1];
        uniform[11] = bone_dir[2];
        uniform[15] = 1.f;  // Enables bone rendering.

        // Next instance.
        ++instances;
        uniform += 16;

        // Only the joint is rendered for leaves, the bone model isn't.
        if (IsLeaf(_skeleton, i)) {
            // Copy current joint's raw matrix.
            std::memcpy(uniform, current.cols, 16 * sizeof(float));

            // Re-use bone_dir to fix the size of the leaf (same as previous bone).
            // The shader expects to find it at index [3,7,11] of the matrix.
            uniform[3] = bone_dir[0];
            uniform[7] = bone_dir[1];
            uniform[11] = bone_dir[2];
            uniform[15] = 0.f;  // Disables bone rendering.
            ++instances;
        }
    }

    return instances;
}
}  // namespace

void SkeletonView::onAwake() {
    _createBoneMesh();
    _createJointMesh();
    _material = std::make_shared<SkeletonMaterial>(scene()->device());
    _skeletonBuffer.resize(animation::Skeleton::kMaxJoints * 64);

    auto boneEntity = entity()->createChild("boneEntity");
    auto boneRenderer = boneEntity->addComponent<MeshRenderer>();
    boneRenderer->setMesh(_boneMesh);
    boneRenderer->setMaterial(_material);

    auto jointEntity = entity()->createChild("jointEntity");
    auto jointRenderer = jointEntity->addComponent<MeshRenderer>();
    jointRenderer->setMesh(_jointMesh);
    jointRenderer->setMaterial(_material);
}

void SkeletonView::onUpdate(float deltaTime) {
    if (!_animator) {
        _animator = entity()->getComponent<Animator>();
    }

    if (_animator) {
        fillPostureUniforms(_animator->skeleton(), make_span(_animator->models()), _skeletonBuffer.data());
    }
}

void SkeletonView::_createBoneMesh() {
    const Vector3F pos[6] = {Vector3F(1.f, 0.f, 0.f),      Vector3F(kInter, .1f, .1f),  Vector3F(kInter, .1f, -.1f),
                             Vector3F(kInter, -.1f, -.1f), Vector3F(kInter, -.1f, .1f), Vector3F(0.f, 0.f, 0.f)};
    const Vector3F normals[8] = {
            (pos[2] - pos[1]).cross(pos[2] - pos[0]).normalized(),
            (pos[1] - pos[2]).cross(pos[1] - pos[5]).normalized(),
            (pos[3] - pos[2]).cross(pos[3] - pos[0]).normalized(),
            (pos[2] - pos[3]).cross(pos[2] - pos[5]).normalized(),
            (pos[4] - pos[3]).cross(pos[4] - pos[0]).normalized(),
            (pos[3] - pos[4]).cross(pos[3] - pos[5]).normalized(),
            (pos[1] - pos[4]).cross(pos[1] - pos[0]).normalized(),
            (pos[4] - pos[1]).cross(pos[4] - pos[5]).normalized(),
    };

    std::vector<Vector3F> position = {pos[0], pos[2], pos[1], pos[5], pos[1], pos[2], pos[0], pos[3],
                                      pos[2], pos[5], pos[2], pos[3], pos[0], pos[4], pos[3], pos[5],
                                      pos[3], pos[4], pos[0], pos[1], pos[4], pos[5], pos[4], pos[1]};
    std::vector<Vector3F> normal = {normals[0], normals[0], normals[0], normals[1], normals[1], normals[1],
                                    normals[2], normals[2], normals[2], normals[3], normals[3], normals[3],
                                    normals[4], normals[4], normals[4], normals[5], normals[5], normals[5],
                                    normals[6], normals[6], normals[6], normals[7], normals[7], normals[7]};

    _boneMesh = MeshManager::GetSingleton().LoadModelMesh();
    _boneMesh->setPositions(position);
    _boneMesh->setNormals(normal);
    _boneMesh->addSubMesh(0, 24);
    _boneMesh->uploadData(true);
}

void SkeletonView::_createJointMesh() {
    const int kNumSlices = 20;
    const int kNumPointsPerCircle = kNumSlices + 1;
    const int kNumPointsYZ = kNumPointsPerCircle;
    const int kNumPointsXY = kNumPointsPerCircle + kNumPointsPerCircle / 4;
    const int kNumPointsXZ = kNumPointsPerCircle;
    const int kNumPoints = kNumPointsXY + kNumPointsXZ + kNumPointsYZ;
    const float kRadius = kInter;  // Radius multiplier.

    std::vector<Vector3F> position(kNumPoints);
    std::vector<Vector3F> normal(kNumPoints);
    std::vector<Color> color(kNumPoints);

    // Fills vertices.
    int index = 0;
    for (int j = 0; j < kNumPointsYZ; ++j) {  // YZ plan.
        float angle = j * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        position[index] = Vector3F(0.f, c * kRadius, s * kRadius);
        normal[index] = Vector3F(0.f, c, s);
        color[index] = Color(1, 0, 0, 1);
        index++;
    }
    for (int j = 0; j < kNumPointsXY; ++j) {  // XY plan.
        float angle = j * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        position[index] = Vector3F(s * kRadius, c * kRadius, 0.f);
        normal[index] = Vector3F(s, c, 0.f);
        color[index] = Color(0, 1, 0, 1);
    }
    for (int j = 0; j < kNumPointsXZ; ++j) {  // XZ plan.
        float angle = j * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        position[index] = Vector3F(c * kRadius, 0.f, -s * kRadius);
        normal[index] = Vector3F(c, 0.f, -s);
        color[index] = Color(0, 0, 1, 1);
    }

    _jointMesh = MeshManager::GetSingleton().LoadModelMesh();
    _jointMesh->setPositions(position);
    _jointMesh->setNormals(normal);
    _jointMesh->setColors(color);
    _jointMesh->addSubMesh(0, kNumPoints, wgpu::PrimitiveTopology::LineStrip);
    _jointMesh->uploadData(true);
}
}  // namespace vox