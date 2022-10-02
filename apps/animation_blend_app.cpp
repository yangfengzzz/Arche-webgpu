//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_blend_app.h"

#include "vox.render/animation/animation_states/animation_blending.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
class CustomGUI : public ui::Widget {
public:
    std::shared_ptr<AnimationClip> clip1{nullptr};
    std::shared_ptr<AnimationClip> clip2{nullptr};
    std::shared_ptr<AnimationClip> clip3{nullptr};

    void DrawImpl() override {
        ImGui::Text("Custom UI");
        ImGui::SliderFloat("Clip1 Wight", &clip1->weight, 0.f, 1.f);
        ImGui::SliderFloat("Clip2 Wight", &clip2->weight, 0.f, 1.f);
        ImGui::SliderFloat("Clip3 Wight", &clip3->weight, 0.f, 1.f);
    }
};
}  // namespace

void AnimationBlendApp::loadScene() {
    _gui->LoadFont("Ruda_Big", "Fonts/Ruda-Bold.ttf", 16);
    _gui->LoadFont("Ruda_Medium", "Fonts/Ruda-Bold.ttf", 14);
    _gui->LoadFont("Ruda_Small", "Fonts/Ruda-Bold.ttf", 12);
    _gui->UseFont("Ruda_Medium");
    _gui->SetEditorLayoutAutosaveFrequency(60.0f);
    _gui->EnableEditorLayoutSave(true);
    _gui->EnableDocking(true);

    _gui->SetCanvas(canvas_);
    canvas_.AddPanel(panel_);
    auto& widget = panel_.CreateWidget<CustomGUI>();

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
    auto animationBlending = std::make_shared<AnimatorBlending>();
    widget.clip1 = std::make_shared<AnimationClip>("Animation/pab_walk.ozz");
    widget.clip2 = std::make_shared<AnimationClip>("Animation/pab_jog.ozz");
    widget.clip3 = std::make_shared<AnimationClip>("Animation/pab_run.ozz");
    animationBlending->addChild(widget.clip1);
    animationBlending->addChild(widget.clip2);
    animationBlending->addChild(widget.clip3);
    animator->setRootState(animationBlending);

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
