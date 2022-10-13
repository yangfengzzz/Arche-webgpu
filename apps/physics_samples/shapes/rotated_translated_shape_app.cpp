//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/rotated_translated_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

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
    }
};

}  // namespace

bool PhysicsRotatedTranslatedShapeApp::prepare(Platform& platform) {
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

void PhysicsRotatedTranslatedShapeApp::loadScene() {
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
        // Floor
        PhysicsUtils::createFloor(body_interface);

        // Create a cone centered on the origin with the point pointing upwards
        Array<Vec3> points;
        points.push_back(Vec3(0, 2.5f, 0));
        for (float a = 0; a < DegreesToRadians(360); a += DegreesToRadians(36))
            points.push_back(Vec3(Sin(a), -2.5f, Cos(a)));
        Ref<ConvexHullShapeSettings> convex_hull = new ConvexHullShapeSettings(points);

        // Offset and rotate so that the cone is upside down on its point
        Ref<RotatedTranslatedShapeSettings> rot_trans = new RotatedTranslatedShapeSettings(
                Vec3(0, 2.5f, 0), Quat::sRotation(Vec3::sAxisX(), JPH_PI), convex_hull);

        // Place at 0 so that the point touches the floor
        Body& body = *body_interface.CreateBody(BodyCreationSettings(
                rot_trans, Vec3::sZero(), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
