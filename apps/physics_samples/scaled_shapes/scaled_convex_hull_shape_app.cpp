//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/scaled_shapes/scaled_convex_hull_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

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
    }
};

}  // namespace

bool PhysicsScaledConvexHullShapeApp::prepare(Platform &platform) {
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

void PhysicsScaledConvexHullShapeApp::loadScene() {
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

        // Create tetrahedron
        Array<Vec3> tetrahedron;
        tetrahedron.push_back(Vec3::sZero());
        tetrahedron.push_back(Vec3(10, 0, 12.5f));
        tetrahedron.push_back(Vec3(15, 0, 2.5f));
        tetrahedron.push_back(Vec3(10, -5, 5));

        // Create vertices for box
        Array<Vec3> box;
        box.push_back(Vec3(1, 2, 3));
        box.push_back(Vec3(-1, 2, 3));
        box.push_back(Vec3(1, -2, 3));
        box.push_back(Vec3(-1, -2, 3));
        box.push_back(Vec3(1, 2, -3));
        box.push_back(Vec3(-1, 2, -3));
        box.push_back(Vec3(1, -2, -3));
        box.push_back(Vec3(-1, -2, -3));

        // Rotate and translate vertices
        Mat44 m = Mat44::sTranslation(Vec3(3.0f, -2.0f, 1.0f)) * Mat44::sRotationY(0.2f * JPH_PI) *
                  Mat44::sRotationZ(0.1f * JPH_PI);
        for (Vec3 &v : box) v = m * v;

        // Create convex hulls
        RefConst<ShapeSettings> hull_shape[2] = {new ConvexHullShapeSettings(tetrahedron),
                                                 new ConvexHullShapeSettings(box)};

        for (int i = 0; i < 2; ++i) {
            // Original shape
            Body &body1 = *body_interface.CreateBody(BodyCreationSettings(hull_shape[i], Vec3(-40, 10, i * 20.0f),
                                                                          Quat::sIdentity(), EMotionType::Dynamic,
                                                                          PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body1.GetID(), EActivation::Activate);

            // Uniformly scaled shape
            Body &body2 = *body_interface.CreateBody(BodyCreationSettings(
                    new ScaledShapeSettings(hull_shape[i], Vec3::sReplicate(0.25f)), Vec3(-20, 10, i * 20.0f),
                    Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body2.GetID(), EActivation::Activate);

            // Non-uniform scaled shape
            Body &body3 = *body_interface.CreateBody(BodyCreationSettings(
                    new ScaledShapeSettings(hull_shape[i], Vec3(0.25f, 0.5f, 1.5f)), Vec3(0, 10, i * 20.0f),
                    Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body3.GetID(), EActivation::Activate);

            // Flipped in 2 axis
            Body &body4 = *body_interface.CreateBody(BodyCreationSettings(
                    new ScaledShapeSettings(hull_shape[i], Vec3(-0.25f, 0.5f, -1.5f)), Vec3(20, 10, i * 20.0f),
                    Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body4.GetID(), EActivation::Activate);

            // Inside out
            Body &body5 = *body_interface.CreateBody(BodyCreationSettings(
                    new ScaledShapeSettings(hull_shape[i], Vec3(-0.25f, 0.5f, 1.5f)), Vec3(40, 10, i * 20.0f),
                    Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body5.GetID(), EActivation::Activate);
        }
    }
    scene->play();
}

}  // namespace vox
