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
#include "vox.render/shader/shader_common.h"
#include "vox.render/shader/shader_manager.h"

namespace vox::skeleton_view {
namespace {
// A vertex made of positions and normals.
struct VertexPNC {
    Vector3F pos;
    Vector3F normal;
    Color color;
};

int fillPostureUniforms(const animation::Skeleton& _skeleton,
                        const span<const simd_math::Float4x4>& _matrices,
                        float* _uniforms) {
    assert(isAligned(_uniforms, alignof(simd_math::SimdFloat4)));

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

SkeletonView::SkeletonView(Entity* entity)
    : Script(entity),
      _skeletonData(animation::Skeleton::kMaxJoints * 64),
      _skeletonBuffer(scene()->device(),
                      _skeletonData.size() * sizeof(float),
                      wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst) {}

void SkeletonView::onAwake() {
    _boneMaterial = std::make_shared<Material>(scene()->device());
    _boneMaterial->vertex_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/bone.vert");
    _boneMaterial->fragment_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/skeleton.frag");

    _jointMaterial = std::make_shared<Material>(scene()->device());
    _jointMaterial->vertex_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/joint.vert");
    _jointMaterial->fragment_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/skeleton.frag");

    _createMeshLayout();
    _createBoneMesh();
    _createJointMesh();

    auto boneEntity = entity()->createChild("boneEntity");
    auto boneRenderer = boneEntity->addComponent<MeshRenderer>();
    boneRenderer->setMesh(_boneMesh);
    boneRenderer->setMaterial(_boneMaterial);

    auto jointEntity = entity()->createChild("jointEntity");
    auto jointRenderer = jointEntity->addComponent<MeshRenderer>();
    jointRenderer->setMesh(_jointMesh);
    jointRenderer->setMaterial(_jointMaterial);
}

void SkeletonView::onUpdate(float deltaTime) {
    if (!_animator) {
        _animator = entity()->getComponent<Animator>();
    }

    if (_animator) {
        int instanceCount =
                fillPostureUniforms(_animator->skeleton(), make_span(_animator->models()), _skeletonData.data());
        _boneMesh->setInstanceCount(instanceCount);
        _jointMesh->setInstanceCount(instanceCount);
        _skeletonBuffer.uploadData(scene()->device(), _skeletonData.data(), _skeletonData.size() * sizeof(float));
    }
}

void SkeletonView::_createMeshLayout() {
    _vertex_attributes.resize(3);
    _vertex_attributes[0].format = wgpu::VertexFormat::Float32x3;
    _vertex_attributes[0].offset = 0;
    _vertex_attributes[0].shaderLocation = (uint32_t)Attributes::POSITION;
    _vertex_attributes[1].format = wgpu::VertexFormat::Float32x3;
    _vertex_attributes[1].offset = 12;
    _vertex_attributes[1].shaderLocation = (uint32_t)Attributes::NORMAL;
    _vertex_attributes[2].format = wgpu::VertexFormat::Float32x4;
    _vertex_attributes[2].offset = 24;
    _vertex_attributes[2].shaderLocation = (uint32_t)Attributes::COLOR_0;

    _instance_attributes.resize(4);
    _instance_attributes[0].format = wgpu::VertexFormat::Float32x4;
    _instance_attributes[0].offset = 0;
    _instance_attributes[0].shaderLocation = 7;
    _instance_attributes[1].format = wgpu::VertexFormat::Float32x4;
    _instance_attributes[1].offset = 16;
    _instance_attributes[1].shaderLocation = 8;
    _instance_attributes[2].format = wgpu::VertexFormat::Float32x4;
    _instance_attributes[2].offset = 32;
    _instance_attributes[2].shaderLocation = 9;
    _instance_attributes[3].format = wgpu::VertexFormat::Float32x4;
    _instance_attributes[3].offset = 48;
    _instance_attributes[3].shaderLocation = 10;

    _layouts.resize(2);
    _layouts[0].arrayStride = 40;
    _layouts[0].attributeCount = 3;
    _layouts[0].attributes = _vertex_attributes.data();
    _layouts[0].stepMode = wgpu::VertexStepMode::Vertex;

    _layouts[1].arrayStride = 64;
    _layouts[1].attributeCount = 4;
    _layouts[1].attributes = _instance_attributes.data();
    _layouts[1].stepMode = wgpu::VertexStepMode::Instance;
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

    const Color white = {1, 1, 1, 1};

    const VertexPNC bones[24] = {{pos[0], normals[0], white}, {pos[2], normals[0], white}, {pos[1], normals[0], white},
                                 {pos[5], normals[1], white}, {pos[1], normals[1], white}, {pos[2], normals[1], white},
                                 {pos[0], normals[2], white}, {pos[3], normals[2], white}, {pos[2], normals[2], white},
                                 {pos[5], normals[3], white}, {pos[2], normals[3], white}, {pos[3], normals[3], white},
                                 {pos[0], normals[4], white}, {pos[4], normals[4], white}, {pos[3], normals[4], white},
                                 {pos[5], normals[5], white}, {pos[3], normals[5], white}, {pos[4], normals[5], white},
                                 {pos[0], normals[6], white}, {pos[1], normals[6], white}, {pos[4], normals[6], white},
                                 {pos[5], normals[7], white}, {pos[4], normals[7], white}, {pos[1], normals[7], white}};

    Buffer buffer(scene()->device(), 24 * 40, wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);
    buffer.uploadData(scene()->device(), &bones[0], 24 * 40);
    _boneBufferBindings.push_back(buffer);
    _boneBufferBindings.push_back(_skeletonBuffer);
    _boneMesh = MeshManager::GetSingleton().LoadBufferMesh();
    _boneMesh->setVertexLayouts(_layouts);
    _boneMesh->setVertexBufferBindings(_boneBufferBindings, 0);
    _boneMesh->addSubMesh(0, 24);
}

void SkeletonView::_createJointMesh() {
    const int kNumSlices = 20;
    const int kNumPointsPerCircle = kNumSlices + 1;
    const int kNumPointsYZ = kNumPointsPerCircle;
    const int kNumPointsXY = kNumPointsPerCircle + kNumPointsPerCircle / 4;
    const int kNumPointsXZ = kNumPointsPerCircle;
    const int kNumPoints = kNumPointsXY + kNumPointsXZ + kNumPointsYZ;
    const float kRadius = kInter;  // Radius multiplier.
    const Color red = {1, 0, 0, 1};
    const Color green = {0, 1, 0, 1};
    const Color blue = {0, 0, 1, 1};
    VertexPNC joints[kNumPoints];

    // Fills vertices.
    int index = 0;
    for (int j = 0; j < kNumPointsYZ; ++j) {  // YZ plan.
        float angle = float(j) * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        VertexPNC& vertex = joints[index++];
        vertex.pos = Vector3F(0.f, c * kRadius, s * kRadius);
        vertex.normal = Vector3F(0.f, c, s);
        vertex.color = red;
    }
    for (int j = 0; j < kNumPointsXY; ++j) {  // XY plan.
        float angle = float(j) * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        VertexPNC& vertex = joints[index++];
        vertex.pos = Vector3F(s * kRadius, c * kRadius, 0.f);
        vertex.normal = Vector3F(s, c, 0.f);
        vertex.color = blue;
    }
    for (int j = 0; j < kNumPointsXZ; ++j) {  // XZ plan.
        float angle = float(j) * kTwoPiF / kNumSlices;
        float s = sinf(angle), c = cosf(angle);
        VertexPNC& vertex = joints[index++];
        vertex.pos = Vector3F(c * kRadius, 0.f, -s * kRadius);
        vertex.normal = Vector3F(c, 0.f, -s);
        vertex.color = green;
    }

    Buffer buffer(scene()->device(), kNumPoints * 40, wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);
    buffer.uploadData(scene()->device(), &joints[0], kNumPoints * 40);
    _jointBufferBindings.push_back(buffer);
    _jointBufferBindings.push_back(_skeletonBuffer);
    _jointMesh = MeshManager::GetSingleton().LoadBufferMesh();
    _jointMesh->setVertexLayouts(_layouts);
    _jointMesh->setVertexBufferBindings(_jointBufferBindings, 0);
    _jointMesh->addSubMesh(0, kNumPoints);
}
}  // namespace vox::skeleton_view
