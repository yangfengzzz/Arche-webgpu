//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/skinning_app.h"

#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
void SkinningApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 1, 3);
    _mainCamera = cameraEntity->addComponent<Camera>();
    auto control = cameraEntity->addComponent<control::OrbitControl>();
    control->target.set(0, 1, 0);

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto characterEntity = rootEntity->createChild();
    auto animator = characterEntity->addComponent<Animator>();
    animator->loadSkeleton("Animation/ruby_skeleton.ozz");
    auto animationClip = std::make_shared<AnimationClip>("Animation/ruby_animation.ozz");
    animator->setRootState(animationClip);

    auto skins = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/ruby_mesh.ozz");
    for (auto& skin : skins) {
        auto renderer = characterEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<BlinnPhongMaterial>(_device);
        material->setBaseColor(Color(0.4, 0.6, 0.6, 0.6));
        material->setIsTransparent(true);
        renderer->setMaterial(material);
    }

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto planeEntity = rootEntity->createChild();
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 10));
    auto texturedMaterial = std::make_shared<UnlitMaterial>(_device);
    texturedMaterial->setRenderFace(RenderFace::Double);
    planeRenderer->setMaterial(texturedMaterial);

    texturedMaterial->setBaseTexture(ImageManager::GetSingleton().loadTexture("Textures/wood.png"));

    scene->play();
}

}  // namespace vox
