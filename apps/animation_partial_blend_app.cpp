//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_partial_blend_app.h"

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
    std::shared_ptr<AnimationClip> samplers[3];

    void DrawImpl() override {
        ImGui::Text("Weight UI");
        ImGui::Checkbox("manual", &manual_);
        if (manual_) {
            ImGui::SliderFloat("Clip Jog Weight", &samplers[0]->weight, 0.f, 1.f);
            ImGui::SliderFloat("Clip Walk Weight", &samplers[1]->weight, 0.f, 1.f);
            ImGui::SliderFloat("Clip Run Weight", &samplers[2]->weight, 0.f, 1.f);
        } else {
            ImGui::SliderFloat("Blend Weight", &blend_ratio_, 0.f, 1.f);
            updateRuntimeParameters();

            ImGui::Text("%s", fmt::format("Clip Jog Weight {}", samplers[0]->weight).c_str());
            ImGui::Text("%s", fmt::format("Clip Walk Weight {}", samplers[1]->weight).c_str());
            ImGui::Text("%s", fmt::format("Clip Run Weight {}", samplers[2]->weight).c_str());
        }
    }

    // Computes blending weight and synchronizes playback speed when the "manual"
    // option is off.
    void updateRuntimeParameters() {
        // Computes weight parameters for all samplers.
        const float kNumIntervals = kNumLayers - 1;
        const float kInterval = 1.f / kNumIntervals;
        for (int i = 0; i < kNumLayers; ++i) {
            const float med = i * kInterval;
            const float x = blend_ratio_ - med;
            const float y = ((x < 0.f ? x : -x) + kInterval) * kNumIntervals;
            samplers[i]->weight = std::max(0.f, y);
        }

        // Synchronizes animations.
        // First computes loop cycle duration. Selects the 2 samplers that define
        // interval that contains blend_ratio_.
        // Uses a maximum value smaller that 1.f (-epsilon) to ensure that
        // (relevant_sampler + 1) is always valid.
        const int relevant_sampler = static_cast<int>((blend_ratio_ - 1e-3f) * (kNumLayers - 1));
        assert(relevant_sampler + 1 < kNumLayers);
        auto sampler_l = samplers[relevant_sampler];
        auto sampler_r = samplers[relevant_sampler + 1];

        // Interpolates animation durations using their respective weights, to
        // find the loop cycle duration that matches blend_ratio_.
        const float loop_duration = sampler_l->animation().duration() * sampler_l->weight +
                                    sampler_r->animation().duration() * sampler_r->weight;

        // Finally finds the speed coefficient for all samplers.
        const float inv_loop_duration = 1.f / loop_duration;
        for (auto sampler : samplers) {
            const float speed = sampler->animation().duration() * inv_loop_duration;
            sampler->playback_speed = speed;
        }
    }

private:
    // The number of layers to blend.
    enum {
        kLowerBody = 0,
        kUpperBody = 1,
        kNumLayers = 2,
    };

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
    widget.samplers[1] = std::make_shared<AnimationClip>("Animation/pab_jog.ozz");
    widget.samplers[2] = std::make_shared<AnimationClip>("Animation/pab_run.ozz");
    animationBlending->addChild(widget.samplers[0]);
    animationBlending->addChild(widget.samplers[1]);
    animationBlending->addChild(widget.samplers[2]);
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
