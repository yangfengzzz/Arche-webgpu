//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/rack_and_pinion_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/Constraints/RackAndPinionConstraint.h>
#include <Jolt/Physics/Constraints/SliderConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_debugger::PhysicsDebugSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool RackAndPinionConstraintApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();

    auto debugger = std::make_unique<physics_debugger::PhysicsDebugSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    return true;
}

void RackAndPinionConstraintApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        PhysicsUtils::createFloor(body_interface);

        constexpr float cGearRadius = 0.5f;
        constexpr float cGearHalfWidth = 0.05f;
        constexpr int cGearNumTeeth = 100;

        constexpr float cRackLength = 10.0f;
        constexpr float cRackHalfHeight = 0.1f;
        constexpr float cRackHalfWidth = 0.05f;
        constexpr int cRackNumTeeth = int(cRackLength * cGearNumTeeth / (2.0f * JPH_PI * cGearRadius));

        constexpr float cToothThicknessBottom = 0.01f;
        constexpr float cToothThicknessTop = 0.005f;
        constexpr float cToothHeight = 0.02f;

        // Create a tooth
        Array<Vec3> tooth_points = {
                Vec3(0, cGearHalfWidth, cToothThicknessBottom),
                Vec3(0, -cGearHalfWidth, cToothThicknessBottom),
                Vec3(0, cGearHalfWidth, -cToothThicknessBottom),
                Vec3(0, -cGearHalfWidth, -cToothThicknessBottom),
                Vec3(cToothHeight, -cGearHalfWidth, cToothThicknessTop),
                Vec3(cToothHeight, cGearHalfWidth, cToothThicknessTop),
                Vec3(cToothHeight, -cGearHalfWidth, -cToothThicknessTop),
                Vec3(cToothHeight, cGearHalfWidth, -cToothThicknessTop),
        };
        ConvexHullShapeSettings tooth_settings(tooth_points);
        tooth_settings.SetEmbedded();

        // Create gear
        CylinderShapeSettings gear_cylinder(cGearHalfWidth, cGearRadius);
        gear_cylinder.SetEmbedded();

        StaticCompoundShapeSettings gear_settings;
        gear_settings.SetEmbedded();

        gear_settings.AddShape(Vec3::sZero(), Quat::sIdentity(), &gear_cylinder);
        for (int i = 0; i < cGearNumTeeth; ++i) {
            Quat rotation = Quat::sRotation(Vec3::sAxisY(), 2.0f * JPH_PI * i / cGearNumTeeth);
            gear_settings.AddShape(rotation * Vec3(cGearRadius, 0, 0), rotation, &tooth_settings);
        }

        Vec3 gear_initial_p(0, 2.0f, 0);
        Quat gear_initial_r = Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI);
        Body *gear = body_interface.CreateBody(BodyCreationSettings(
                &gear_settings, gear_initial_p, gear_initial_r, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(gear->GetID(), EActivation::Activate);

        // Create rack
        BoxShapeSettings rack_box(Vec3(cRackHalfHeight, cRackHalfWidth, 0.5f * cRackLength), 0.0f);
        rack_box.SetEmbedded();

        StaticCompoundShapeSettings rack_settings;
        rack_settings.SetEmbedded();

        rack_settings.AddShape(Vec3::sZero(), Quat::sIdentity(), &rack_box);
        for (int i = 0; i < cRackNumTeeth; ++i)
            rack_settings.AddShape(
                    Vec3(cRackHalfHeight, 0, -0.5f * cRackLength + (i + 0.5f) * cRackLength / cRackNumTeeth),
                    Quat::sIdentity(), &tooth_settings);

        Vec3 slider_initial_p = gear_initial_p - Vec3(0, cGearRadius + cRackHalfHeight + cToothHeight, 0);
        Quat slider_initial_r = Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI) * gear_initial_r;
        Body *rack =
                body_interface.CreateBody(BodyCreationSettings(&rack_settings, slider_initial_p, slider_initial_r,
                                                               EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(rack->GetID(), EActivation::Activate);

        // Create a hinge for the gear
        HingeConstraintSettings hinge;
        hinge.mPoint1 = hinge.mPoint2 = gear_initial_p;
        hinge.mHingeAxis1 = hinge.mHingeAxis2 = Vec3::sAxisZ();
        hinge.mNormalAxis1 = hinge.mNormalAxis2 = Vec3::sAxisX();
        Constraint *hinge_constraint = hinge.Create(Body::sFixedToWorld, *gear);
        PhysicsManager::GetSingleton().addConstraint(hinge_constraint);

        // Create a slider for the rack
        SliderConstraintSettings slider;
        slider.mPoint1 = slider.mPoint2 = gear_initial_p;
        slider.mSliderAxis1 = slider.mSliderAxis2 = Vec3::sAxisX();
        slider.mNormalAxis1 = slider.mNormalAxis2 = Vec3::sAxisZ();
        slider.mLimitsMin = -0.5f * cRackLength;
        slider.mLimitsMax = 0.5f * cRackLength;
        Constraint *slider_constraint = slider.Create(Body::sFixedToWorld, *rack);
        PhysicsManager::GetSingleton().addConstraint(slider_constraint);

        // Disable collision between rack and gear (we want the rack and pinion constraint to take care of the relative
        // movement)
        Ref<GroupFilterTable> group_filter = new GroupFilterTable(2);
        group_filter->DisableCollision(0, 1);
        gear->SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        rack->SetCollisionGroup(CollisionGroup(group_filter, 0, 1));

        // Create rack and pinion constraint to constrain the two bodies
        RackAndPinionConstraintSettings randp;
        randp.mHingeAxis = hinge.mHingeAxis1;
        randp.mSliderAxis = slider.mSliderAxis2;
        randp.SetRatio(cRackNumTeeth, cRackLength, cGearNumTeeth);
        RackAndPinionConstraint *randp_constraint = static_cast<RackAndPinionConstraint *>(randp.Create(*gear, *rack));
        randp_constraint->SetConstraints(hinge_constraint, slider_constraint);
        PhysicsManager::GetSingleton().addConstraint(randp_constraint);

        // Give the gear a spin
        gear->SetAngularVelocity(Vec3(0, 0, 6.0f));
    }
    scene->play();
}

}  // namespace vox
