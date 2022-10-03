//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_ik_foot_app.h"

#include "vox.base/logging.h"
#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
class FloorTargetScript : public Script {
public:
    Animator* animator{nullptr};
    std::vector<std::shared_ptr<Skin>> floor;

    explicit FloorTargetScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {}
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

void AnimationIKFootApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto ibl_map = ImageManager::GetSingleton().generateIBL("Textures/uffizi_rgba16f_cube.ktx");
    auto sh = ImageManager::GetSingleton().generateSH("Textures/uffizi_rgba16f_cube.ktx");
    scene->ambientLight()->setSpecularTexture(ibl_map);
    scene->ambientLight()->setSpecularIntensity(0.2);
    scene->ambientLight()->setDiffuseMode(DiffuseMode::SphericalHarmonics);
    scene->ambientLight()->setDiffuseSphericalHarmonics(sh);

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 4, -6);
    _mainCamera = cameraEntity->addComponent<Camera>();
    auto control = cameraEntity->addComponent<control::OrbitControl>();
    control->target.set(2.17f, 3.f, -2.06f);

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(2.17f, 2.f, -2.06f);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto target = rootEntity->createChild("target");
    auto targetScript = target->addComponent<FloorTargetScript>();
    auto targetRenderer = target->addComponent<MeshRenderer>();
    targetRenderer->setMesh(PrimitiveMesh::createSphere(_device, 0.05));
    targetRenderer->setMaterial(std::make_shared<BlinnPhongMaterial>(_device));

    auto characterEntity = rootEntity->createChild();
    characterEntity->transform->setPosition(2.17f, 2.f, -2.06f);
    auto animator = characterEntity->addComponent<Animator>();
    animator->loadSkeleton("Animation/pab_skeleton.ozz");
    auto animationClip = std::make_shared<AnimationClip>("Animation/pab_crossarms.ozz");
    animator->setRootState(animationClip);
    targetScript->animator = animator;

    auto skins = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/arnaud_mesh.ozz");
    for (auto& skin : skins) {
        auto renderer = characterEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<BlinnPhongMaterial>(_device);
        material->setBaseColor(Color(0.4, 0.6, 0.6, 0.6));
        material->setIsTransparent(true);
        renderer->setMaterial(material);
    }

    auto floorEntity = rootEntity->createChild();
    targetScript->floor = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/floor.ozz");
    for (auto& skin : targetScript->floor) {
        auto renderer = floorEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<PBRMaterial>(_device);
        material->setRoughness(1);
        material->setMetallic(0);
        renderer->setMaterial(material);
    }

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto attachEntity = characterEntity->createChild();
    auto attachRenderer = attachEntity->addComponent<MeshRenderer>();
    attachRenderer->setMesh(PrimitiveMesh::createCuboid(_device, 0.01, 0.01, 1));
    auto attachMtl = std::make_shared<BlinnPhongMaterial>(_device);
    attachMtl->setBaseColor(Color(1, 0, 0, 1));
    attachRenderer->setMaterial(attachMtl);
    animator->bindEntity("LeftHandMiddle1", attachEntity);

    scene->play();
}

}  // namespace vox
