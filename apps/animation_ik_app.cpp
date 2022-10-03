//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_ik_app.h"

#include "vox.render/animation/animator.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
class TargetScript : public Script {
public:
    Animator* animator{nullptr};
    float totalTime = 0.f;
    float target_extent = 0.5f;
    Point3F target_offset{0.f, .2f, .1f};

    Vector3F root_translation{0, 0, 0};
    Vector3F root_euler{0, 0, 0};
    float root_scale = 1.f;
    Vector3F pole_vector{0, 1, 0};
    float weight = 1.f;
    float soften = .97f;
    float twist_angle = 0.f;

    int start_joint = -1;
    int mid_joint = -1;
    int end_joint = -1;

    explicit TargetScript(Entity* entity) : Script(entity) {}

    [[nodiscard]] simd_math::Float4x4 GetRootTransform() const {
        return simd_math::Float4x4::Translation(simd_math::simd_float4::Load3PtrU(&root_translation.x)) *
               simd_math::Float4x4::FromEuler(simd_math::simd_float4::Load3PtrU(&root_euler.x)) *
               simd_math::Float4x4::Scaling(simd_math::simd_float4::Load1(root_scale));
    }

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        const float anim_extent = (1.f - std::cos(totalTime)) * .5f * target_extent;
        const int floor = static_cast<int>(std::fabs(totalTime) / kTwoPiF);

        auto target = target_offset;
        (&target.x)[floor % 3] += anim_extent;
        entity()->transform->setPosition(target);

        // Target and pole should be in model-space, so they must be converted from
        // world-space using character inverse root matrix.
        // IK jobs must support non invertible matrices (like 0 scale matrices).
        simd_math::SimdInt4 invertible;
        const simd_math::Float4x4 invert_root = Invert(GetRootTransform(), &invertible);

        const simd_math::SimdFloat4 target_ms =
                TransformPoint(invert_root, simd_math::simd_float4::Load3PtrU(&target.x));
        const simd_math::SimdFloat4 pole_vector_ms =
                TransformVector(invert_root, simd_math::simd_float4::Load3PtrU(&pole_vector.x));

        // Setup IK job.
        animation::IKTwoBoneJob ik_job;
        ik_job.target = target_ms;
        ik_job.pole_vector = pole_vector_ms;
        ik_job.mid_axis = simd_math::simd_float4::z_axis();  // Middle joint
                                                             // rotation axis is
                                                             // fixed, and depends
                                                             // on skeleton rig.
        ik_job.weight = weight;
        ik_job.soften = soften;
        ik_job.twist_angle = twist_angle;
        animator->scheduleTwoBoneIK(ik_job, {start_joint, mid_joint, end_joint});
    }
};

}  // namespace

void AnimationIKApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(2, 2, 6);
    _mainCamera = cameraEntity->addComponent<Camera>();
    auto control = cameraEntity->addComponent<control::OrbitControl>();
    control->target.set(0, 1, 0);

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto target = rootEntity->createChild("target");
    auto targetScript = target->addComponent<TargetScript>();
    auto targetRenderer = target->addComponent<MeshRenderer>();
    targetRenderer->setMesh(PrimitiveMesh::createSphere(_device, 0.05));
    targetRenderer->setMaterial(std::make_shared<BlinnPhongMaterial>(_device));

    auto characterEntity = rootEntity->createChild();
    auto animator = characterEntity->addComponent<Animator>();
    animator->loadSkeleton("Animation/robot_skeleton.ozz");
    targetScript->animator = animator;

    // Find the 3 joints in skeleton hierarchy.
    for (int i = 0; i < animator->skeleton().num_joints(); i++) {
        const char* joint_name = animator->skeleton().joint_names()[i];
        if (std::strcmp(joint_name, "shoulder") == 0) {
            targetScript->start_joint = i;
        } else if (std::strcmp(joint_name, "forearm") == 0) {
            targetScript->mid_joint = i;
        } else if (std::strcmp(joint_name, "wrist") == 0) {
            targetScript->end_joint = i;
        }
    }

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto planeEntity = rootEntity->createChild();
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 10));
    auto texturedMaterial = std::make_shared<BlinnPhongMaterial>(_device);
    texturedMaterial->setRenderFace(RenderFace::Double);
    planeRenderer->setMaterial(texturedMaterial);

    texturedMaterial->setBaseTexture(ImageManager::GetSingleton().loadTexture("Textures/wood.png"));

    scene->play();
}

}  // namespace vox
