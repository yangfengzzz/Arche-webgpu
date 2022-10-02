//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_lookat_app.h"

#include "vox.base/logging.h"
#include "vox.render/animation/animation_states/animation_blending.h"
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
class MoveScript : public Script {
public:
    int chain_length = 4;
    float joint_weight = 0.5;
    Animator* animator{nullptr};
    Point3F target_offset = Point3F(.2f, 1.5f, -.3f);
    float target_extent = 1.f;
    float totalTime = 0.f;

    // Indices of the joints that are IKed for look-at purpose.
    // Joints must be from the same hierarchy (all ancestors of the first joint
    // listed) and ordered from child to parent.
    int joints_chain_[4]{};

    explicit MoveScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        const Vector3F animated_target(std::sin(totalTime * .5f), std::cos(totalTime * .25f),
                                       std::cos(totalTime) * .5f + .5f);
        auto target = target_offset + animated_target * target_extent;
        entity()->transform->setPosition(target);

        // The algorithm iteratively updates from the first joint (closer to the
        // head) to the last (the further ancestor, closer to the pelvis). Joints
        // order is already validated. For the first joint, aim IK is applied with
        // the global forward and offset, so the forward vector aligns in direction
        // of the target. If a weight lower that 1 is provided to the first joint,
        // then it will not fully align to the target. In this case further joint
        // will need to be updated. For the remaining joints, forward vector and
        // offset position are computed in each joint local-space, before IK is
        // applied:
        // 1. Rotates forward and offset position based on the result of the
        // previous joint IK.
        // 2. Brings forward and offset back in joint local-space.
        // Aim is iteratively applied up to the last selected joint of the
        // hierarchy. A weight of 1 is given to the last joint so we can guarantee
        // target is reached. Note that model-space transform of each joint doesn't
        // need to be updated between each pass, as joints are ordered from child to
        // parent.
        int previous_joint = animation::Skeleton::kNoParent;
        for (int i = 0, joint = joints_chain_[0]; i < chain_length;
             ++i, previous_joint = joint, joint = joints_chain_[i]) {
            // Setups weights of IK job.
            // the last joint being processed needs a full weight (1.f) to ensure
            // target is reached.
            const bool last = i == chain_length - 1;
            float weight = float(chain_length) * (last ? 1.f : joint_weight);

            animator->scheduleAimIK(joint, target, weight);
        }
    }
};

// Traverses the hierarchy from the first joint to the root, to check if
// joints are all ancestors (same branch), and ordered.
bool validateJointsOrder(const animation::Skeleton& _skeleton, span<const int> _joints) {
    const size_t count = _joints.size();
    if (count == 0) {
        return true;
    }

    size_t i = 1;
    for (int joint = _joints[0], parent = _skeleton.joint_parents()[joint];
         i != count && joint != animation::Skeleton::kNoParent;
         joint = parent, parent = _skeleton.joint_parents()[joint]) {
        if (parent == _joints[i]) {
            ++i;
        }
    }

    return count == i;
}

}  // namespace

void AnimationLookAtApp::loadScene() {
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
    auto targetScript = target->addComponent<MoveScript>();
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
            targetScript->joints_chain_[found] = i;

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
    if (!validateJointsOrder(skeleton, targetScript->joints_chain_)) {
        LOGE("Joints aren't properly ordered, they must be from "
             "the same hierarchy (all ancestors of the first joint "
             "listed) and ordered from child to parent.");
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
