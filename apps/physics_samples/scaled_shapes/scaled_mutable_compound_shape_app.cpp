//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/scaled_shapes/scaled_mutable_compound_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_view::PhysicsViewSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }
};

}  // namespace

bool PhysicsScaledMutableCompoundShapeApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();

    auto debugger = std::make_unique<physics_view::PhysicsViewSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    return true;
}

void PhysicsScaledMutableCompoundShapeApp::loadScene() {
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

        // Left end
        Array<Vec3> end1;
        end1.push_back(Vec3(0, 0, 0));
        end1.push_back(Vec3(0, 0, 1));
        end1.push_back(Vec3(2, 0, 0));
        end1.push_back(Vec3(2, 0, 1));
        end1.push_back(Vec3(0, 1, 0));
        end1.push_back(Vec3(0, 1, 1));
        end1.push_back(Vec3(2, 1, 0));
        end1.push_back(Vec3(2, 1, 1));
        RefConst<ShapeSettings> end1_shape = new ConvexHullShapeSettings(end1);

        // Right end
        Array<Vec3> end2;
        end2.push_back(Vec3(0, 0, 0));
        end2.push_back(Vec3(0, 0, 5));
        end2.push_back(Vec3(0, 1, 0));
        end2.push_back(Vec3(0, 1, 5));
        end2.push_back(Vec3(1, 0, 0));
        end2.push_back(Vec3(1, 0, 5));
        end2.push_back(Vec3(1, 1, 0));
        end2.push_back(Vec3(1, 1, 5));
        RefConst<ShapeSettings> end2_shape = new ConvexHullShapeSettings(end2);

        // Central part
        Array<Vec3> center;
        center.push_back(Vec3(0, 0, 0));
        center.push_back(Vec3(0, 0, 1));
        center.push_back(Vec3(0, 1, 0));
        center.push_back(Vec3(0, 1, 1));
        center.push_back(Vec3(10, 0, 0));
        center.push_back(Vec3(10, 0, 1));
        center.push_back(Vec3(10, 1, 0));
        center.push_back(Vec3(10, 1, 1));
        RefConst<ShapeSettings> center_shape = new ConvexHullShapeSettings(center);

        // Create compound
        Ref<MutableCompoundShapeSettings> compound_shape = new MutableCompoundShapeSettings;
        compound_shape->AddShape(Vec3(-5, -1.5f, -0.5f), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI), end1_shape);
        compound_shape->AddShape(Vec3(5, -0.5f, -0.5f), Quat::sIdentity(), end2_shape);
        compound_shape->AddShape(Vec3(-5, -0.5f, -0.5f), Quat::sIdentity(), center_shape);

        // Original shape
        Body &body1 = *body_interface.CreateBody(BodyCreationSettings(
                compound_shape, Vec3(-40, 10, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body1.GetID(), EActivation::Activate);

        // Uniformly scaled shape
        Body &body2 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(compound_shape, Vec3::sReplicate(0.25f)), Vec3(-20, 10, 0),
                                     Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        // Non-uniform scaled shape
        Body &body3 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(compound_shape, Vec3(0.25f, 0.5f, 1.5f)), Vec3(0, 10, 0),
                                     Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body3.GetID(), EActivation::Activate);

        // Flipped in 2 axis
        Body &body4 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(compound_shape, Vec3(-0.25f, 0.5f, -1.5f)),
                                     Vec3(20, 10, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body4.GetID(), EActivation::Activate);

        // Inside out
        Body &body5 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(compound_shape, Vec3(-0.25f, 0.5f, 1.5f)), Vec3(40, 10, 0),
                                     Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body5.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
