//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation/animation_partial_blend_app.h"

#include <spdlog/fmt/chrono.h>

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
    // Index of the joint at the base of the upper body hierarchy.
    int upper_body_root{0};

    std::shared_ptr<AnimationClip> samplers[2];

    void DrawImpl() override {
        ImGui::Text("Weight UI");
        ImGui::Checkbox("manual", &manual_);
        if (manual_) {
            ImGui::SliderFloat("Clip Lower Weight", &samplers[0]->weight, 0.f, 1.f);
            ImGui::SliderFloat("Clip Lower Joint Mask", &lowerMask, 0.f, 1.f);
            ImGui::SliderFloat("Clip Upper Weight", &samplers[1]->weight, 0.f, 1.f);
            ImGui::SliderFloat("Clip Upper Joint Mask", &upperMask, 0.f, 1.f);
        } else {
            ImGui::SliderFloat("Global Weight", &coefficient, 0.f, 1.f);
            samplers[0]->weight = 1.f;
            lowerMask = 1.f - coefficient;
            samplers[1]->weight = 1.f;
            upperMask = coefficient;

            ImGui::Text("%s", fmt::format("Clip Lower Weight {}", samplers[0]->weight).c_str());
            ImGui::Text("%s", fmt::format("Clip Lower Joint Mask {}", lowerMask).c_str());
            ImGui::Text("%s", fmt::format("Clip Upper Weight {}", samplers[1]->weight).c_str());
            ImGui::Text("%s", fmt::format("Clip Upper Joint Mask {}", upperMask).c_str());
        }
        setupPerJointWeights();
    }

    void setupPerJointWeights() {
        // Setup partial animation mask. This mask is defined by a weight_setting
        // assigned to each joint of the hierarchy. Joint to disable are set to a
        // weight_setting of 0.f, and enabled joints are set to 1.f.
        // Per-joint weights of lower and upper body layers have opposed values
        // (weight_setting and 1 - weight_setting) in order for a layer to select
        // joints that are rejected by the other layer.
        auto& lower_body_sampler = samplers[kLowerBody];
        auto& upper_body_sampler = samplers[kUpperBody];

        lower_body_sampler->setJointMasks(1.f);
        lower_body_sampler->setJointMasks(lowerMask, "Spine1");
        upper_body_sampler->setJointMasks(0.f);
        upper_body_sampler->setJointMasks(upperMask, "Spine1");
    }

private:
    // The number of layers to blend.
    enum {
        kLowerBody = 0,
        kUpperBody = 1,
        kNumLayers = 2,
    };

    float lowerMask{0.f};
    float upperMask{1.f};
    float coefficient = 1.f;

    // Global blend ratio in range [0,1] that controls all blend parameters and
    // synchronizes playback speeds. A value of 0 gives full weight to the first
    // animation, and 1 to the last.
    float blend_ratio_ = 0.3f;

    // Switch to manual control of animations and blending parameters.
    bool manual_ = false;
};
}  // namespace

void AnimationPartialBlendApp::loadScene() {
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
    cameraEntity->transform->setPosition(2, 2, 6);
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
    widget.samplers[0] = std::make_shared<AnimationClip>("Animation/pab_walk.ozz");
    widget.samplers[0]->weight = 1.f;
    widget.samplers[1] = std::make_shared<AnimationClip>("Animation/pab_crossarms.ozz");
    widget.samplers[1]->weight = 1.f;
    animationBlending->addChild(widget.samplers[0]);
    animationBlending->addChild(widget.samplers[1]);
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
