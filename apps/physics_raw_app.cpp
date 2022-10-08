//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_raw_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    JPH::BodyID sphere_id;
    physics_debugger::PhysicsDebugSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity* entity) : Script(entity) {
        inSettings.mDrawShape = true;
    }

    void onPhysicsUpdate() override {
        step++;

        JPH::BodyInterface& body_interface = PhysicsManager::GetSingleton().getBodyInterface();
        // Output current position and velocity of the sphere
        JPH::Vec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
        JPH::Vec3 velocity = body_interface.GetLinearVelocity(sphere_id);
        std::cout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", "
                  << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", "
                  << velocity.GetZ() << ")" << std::endl;

        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }

private:
    uint step = 0;
};

}  // namespace

bool PhysicsRawApp::prepare(Platform& platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();
    showScript->sphere_id = sphere_id;

    auto debugger = std::make_unique<physics_debugger::PhysicsDebugSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    return true;
}

void PhysicsRawApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface& body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        // Next we can create a rigid body to serve as the floor, we make a large box
        // Create the settings for the collision volume (the shape).
        // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
        JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));

        // Create the shape
        // We don't expect an error here, but you can check
        // floor_shape_result for HasError() / GetError()
        JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
        JPH::ShapeRefC floor_shape = floor_shape_result.Get();

        // Create the settings for the body itself. Note that here you can also set other properties like the
        // restitution / friction.
        JPH::BodyCreationSettings floor_settings(floor_shape, JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(),
                                                 JPH::EMotionType::Static, PhysicsManager::Layers::NON_MOVING);

        // Create the actual rigid body
        // Note that if we run out of bodies this can return nullptr
        JPH::Body* floor = body_interface.CreateBody(floor_settings);

        // Add it to the world
        body_interface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

        // Now create a dynamic body to bounce on the floor
        // Note that this uses the shorthand version of creating and adding a body to the world
        JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::Vec3(0.0f, 2.0f, 0.0f),
                                                  JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic,
                                                  PhysicsManager::Layers::MOVING);
        sphere_id = body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);

        // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
        // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding
        // it to the physics system)
        body_interface.SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));
    }
    scene->play();
}

}  // namespace vox
