//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_attach_app.h"

#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
void AnimationAttachApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(3, 2, 3);
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
    animator->loadSkeleton("Animation/pab_skeleton.ozz");
    auto animationClip = std::make_shared<AnimationClip>("Animation/pab_walk.ozz");
    animator->setRootState(animationClip);

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto attachEntity = rootEntity->createChild();
    auto attachRenderer = attachEntity->addComponent<MeshRenderer>();
    attachRenderer->setMesh(PrimitiveMesh::createCuboid(_device, 0.01, 0.01, 1));
    auto attachMtl = std::make_shared<UnlitMaterial>(_device);
    attachMtl->setBaseColor(Color(1, 0, 0, 1));
    attachRenderer->setMaterial(attachMtl);
    animator->bindEntity("LeftHandMiddle1", attachEntity);

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
