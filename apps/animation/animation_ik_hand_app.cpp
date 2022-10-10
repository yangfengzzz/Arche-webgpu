//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation/animation_ik_hand_app.h"

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

    Animator::HandIKData data;

    explicit TargetScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        const float anim_extent = (1.f - std::cos(totalTime)) * .5f * target_extent;
        const int floor = static_cast<int>(std::fabs(totalTime) / kTwoPiF);

        data.target = target_offset;
        (&data.target.x)[floor % 3] += anim_extent;
        entity()->transform->setPosition(data.target);

        animator->encodeHandIKData(data);
    }
};

}  // namespace

void AnimationIKHandApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(1, 1, 1);
    _mainCamera = cameraEntity->addComponent<Camera>();
    auto control = cameraEntity->addComponent<control::OrbitControl>();
    control->target.set(0, 0.5, 0);

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto target = rootEntity->createChild("target");
    auto targetScript = target->addComponent<TargetScript>();
    auto targetRenderer = target->addComponent<MeshRenderer>();
    targetRenderer->setMesh(PrimitiveMesh::createSphere(_device, 0.01));
    targetRenderer->setMaterial(std::make_shared<BlinnPhongMaterial>(_device));

    auto characterEntity = rootEntity->createChild();
    auto animator = characterEntity->addComponent<Animator>();
    animator->loadSkeleton("Animation/robot_skeleton.ozz");
    targetScript->animator = animator;

    // Find the 3 joints in skeleton hierarchy.
    for (int i = 0; i < animator->skeleton().num_joints(); i++) {
        const char* joint_name = animator->skeleton().joint_names()[i];
        if (std::strcmp(joint_name, "shoulder") == 0) {
            targetScript->data.start_joint = i;
        } else if (std::strcmp(joint_name, "forearm") == 0) {
            targetScript->data.mid_joint = i;
        } else if (std::strcmp(joint_name, "wrist") == 0) {
            targetScript->data.end_joint = i;
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
