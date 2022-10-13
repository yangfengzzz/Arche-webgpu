//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/distance_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_view::PhysicsViewSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity* entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsDistanceConstraintApp::prepare(Platform& platform) {
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

void PhysicsDistanceConstraintApp::loadScene() {
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

        float half_cylinder_height = 2.5f;

        // Variation 0: Fixed distance
        // Variation 1: Min/max distance
        for (int variation = 0; variation < 2; ++variation) {
            // Bodies attached through distance constraints
            Quat rotation = Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI);
            Vec3 position(0, 75, 10.0f * variation);
            Body& top = *body_interface.CreateBody(BodyCreationSettings(new CapsuleShape(half_cylinder_height, 1),
                                                                        position, rotation, EMotionType::Static,
                                                                        PhysicsManager::Layers::NON_MOVING));
            body_interface.AddBody(top.GetID(), EActivation::DontActivate);

            Body* prev = &top;
            for (int i = 1; i < 15; ++i) {
                position += Vec3(5.0f + 2.0f * half_cylinder_height, 0, 0);

                Body& segment = *body_interface.CreateBody(
                        BodyCreationSettings(new CapsuleShape(half_cylinder_height, 1), position, rotation,
                                             EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(segment.GetID(), EActivation::Activate);

                DistanceConstraintSettings settings;
                settings.mPoint1 = position - Vec3(5.0f + half_cylinder_height, 0, 0);
                settings.mPoint2 = position - Vec3(half_cylinder_height, 0, 0);

                if (variation == 1) {
                    // Default distance is 5, override min/max range
                    settings.mMinDistance = 4.0f;
                    settings.mMaxDistance = 8.0f;
                }

                PhysicsManager::GetSingleton().addConstraint(settings.Create(*prev, segment));

                prev = &segment;
            }
        }
    }
    scene->play();
}

}  // namespace vox
