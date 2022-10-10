//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/pulley_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Constraints/PulleyConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_debugger::PhysicsDebugSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity* entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsPulleyConstraintApp::prepare(Platform& platform) {
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

void PhysicsPulleyConstraintApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface& body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        PhysicsUtils::createFloor(body_interface);

        // Variation 0: Max length (rope)
        // Variation 1: Fixed length (rigid rod)
        // Variation 2: Min/max length
        // Variation 3: With ratio (block and tackle)
        for (int variation = 0; variation < 4; ++variation) {
            Vec3 position1(-10, 10, -10.0f * variation);
            Body& body1 = *body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.5f)), position1, Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body1.GetID(), EActivation::Activate);

            Vec3 position2(10, 10, -10.0f * variation);
            Body& body2 = *body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.5f)), position2, Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body2.GetID(), EActivation::Activate);

            PulleyConstraintSettings settings;
            settings.mBodyPoint1 = position1 + Vec3(0, 0.5f, 0);  // Connect at the top of the block
            settings.mBodyPoint2 = position2 + Vec3(0, 0.5f, 0);
            settings.mFixedPoint1 = settings.mBodyPoint1 + Vec3(0, 10, 0);
            settings.mFixedPoint2 = settings.mBodyPoint2 + Vec3(0, 10, 0);

            switch (variation) {
                case 0:
                    // Can't extend but can contract
                    break;

                case 1:
                    // Fixed size
                    settings.mMinLength = settings.mMaxLength = -1;
                    break;

                case 2:
                    // With range
                    settings.mMinLength = 18.0f;
                    settings.mMaxLength = 22.0f;
                    break;

                case 3:
                    // With ratio
                    settings.mRatio = 4.0f;
                    break;
            }

            PhysicsManager::GetSingleton().addConstraint(settings.Create(body1, body2));
        }
    }
    scene->play();
}

}  // namespace vox
