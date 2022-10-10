//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation/animation_ik_lookat_app.h"

#include "vox.base/logging.h"
#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
class TargetScript : public Script {
public:
    Animator* animator{nullptr};
    Point3F target_offset = Point3F(.2f, 1.5f, -.3f);
    float target_extent = 1.f;
    float totalTime = 0.f;

    // Indices of the joints that are IKed for look-at purpose.
    // Joints must be from the same hierarchy (all ancestors of the first joint
    // listed) and ordered from child to parent.
    std::vector<int> joints_chain;

    explicit TargetScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        const Vector3F animated_target(std::sin(totalTime * .5f), std::cos(totalTime * .25f),
                                       std::cos(totalTime) * .5f + .5f);
        auto target = target_offset + animated_target * target_extent;
        entity()->transform->setPosition(target);

        Animator::LookAtIKData data;
        data.target = target;
        data.joints_chain = joints_chain;
        animator->encodeLookAtIK(data);
    }
};

// Traverses the hierarchy from the first joint to the root, to check if
// joints are all ancestors (same branch), and ordered.
bool validateJointsOrder(const animation::Skeleton& _skeleton, const std::vector<int>& joints) {
    const size_t count = joints.size();
    if (count == 0) {
        return true;
    }

    size_t i = 1;
    for (int joint = joints[0], parent = _skeleton.joint_parents()[joint];
         i != count && joint != animation::Skeleton::kNoParent;
         joint = parent, parent = _skeleton.joint_parents()[joint]) {
        if (parent == joints[i]) {
            ++i;
        }
    }

    return count == i;
}

}  // namespace

void AnimationIKLookAtApp::loadScene() {
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
    animator->loadSkeleton("Animation/pab_skeleton.ozz");
    auto animationClip = std::make_shared<AnimationClip>("Animation/pab_crossarms.ozz");
    animator->setRootState(animationClip);
    targetScript->animator = animator;

    // Defines IK chain joint names.
    // Joints must be from the same hierarchy (all ancestors of the first joint
    // listed) and ordered from child to parent.
    const char* kJointNames[4] = {"Head", "Spine3", "Spine2", "Spine1"};
    auto& skeleton = animator->skeleton();
    // Look for each joint in the chain.
    int found = 0;
    for (int i = 0; i < skeleton.num_joints() && found != 4; ++i) {
        const char* joint_name = skeleton.joint_names()[i];
        if (std::strcmp(joint_name, kJointNames[found]) == 0) {
            targetScript->joints_chain.push_back(i);

            // Restart search
            ++found;
            i = 0;
        }
    }
    // Exit if all joints weren't found.
    if (found != 4) {
        LOGE("At least a joint wasn't found in the skeleton hierarchy.")
    }

    // Validates joints are order from child to parent of the same hierarchy.
    if (!validateJointsOrder(skeleton, targetScript->joints_chain)) {
        LOGE("Joints aren't properly ordered, they must be from "
             "the same hierarchy (all ancestors of the first joint "
             "listed) and ordered from child to parent.")
    }

    auto skins = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/arnaud_mesh.ozz");
    for (auto& skin : skins) {
        auto renderer = characterEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<BlinnPhongMaterial>(_device);
        material->setBaseColor(Color(0.4, 0.6, 0.6, 0.6));
        material->setIsTransparent(true);
        renderer->setMaterial(material);
    }

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto attachEntity = rootEntity->createChild();
    auto attachRenderer = attachEntity->addComponent<MeshRenderer>();
    attachRenderer->setMesh(PrimitiveMesh::createCuboid(_device, 0.01, 0.01, 1));
    auto attachMtl = std::make_shared<BlinnPhongMaterial>(_device);
    attachMtl->setBaseColor(Color(1, 0, 0, 1));
    attachRenderer->setMaterial(attachMtl);
    animator->bindEntity("LeftHandMiddle1", attachEntity);

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
