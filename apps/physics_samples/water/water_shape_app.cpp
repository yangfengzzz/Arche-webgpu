//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/water/water_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

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
        // Draw the water surface 5mm below actual surface to avoid z fighting with intersection shapes
        Vec3 surface_point = Vec3(0, 10, 0);
        for (int i = -20; i <= 20; ++i) {
            auto pt = surface_point + Vec3(5.0f * i, 0, -100);
            auto pt2 = surface_point + Vec3(5.0f * i, 0, 100);
            _debugger->DrawLine({pt.GetX(), pt.GetY(), pt.GetZ()}, {pt2.GetX(), pt2.GetY(), pt2.GetZ()},
                                JPH::Color::sBlue);

            pt = surface_point + Vec3(-100, 0, 5.0f * i);
            pt2 = surface_point + Vec3(100, 0, 5.0f * i);
            _debugger->DrawLine({pt.GetX(), pt.GetY(), pt.GetZ()}, {pt2.GetX(), pt2.GetY(), pt2.GetZ()},
                                JPH::Color::sBlue);
        }

        // Broadphase results, will apply buoyancy to any body that intersects with the water volume
        class MyCollector : public CollideShapeBodyCollector {
        public:
            explicit MyCollector(const Plane& inSurface) : mSurface(inSurface) {}

            void AddHit(const BodyID& inBodyID) override {
                auto physicsManager = PhysicsManager::GetSingletonPtr();

                BodyLockWrite lock(physicsManager->getBodyLockInterface(), inBodyID);
                Body& body = lock.GetBody();
                if (body.IsActive()) {
                    auto g = physicsManager->getGravity();
                    body.ApplyBuoyancyImpulse(mSurface, 1.1f, 0.3f, 0.05f, Vec3::sZero(), Vec3(g.x, g.y, g.z),
                                              PhysicsManager::fixed_time_step_);
                }
            }

        private:
            Plane mSurface;
        };
        Plane surface = Plane::sFromPointAndNormal(surface_point, Vec3::sAxisY());
        MyCollector collector(surface);

        // Apply buoyancy to all bodies that intersect with the water
        AABox water_box(surface_point - Vec3(100, 100, 100), surface_point + Vec3(100, 0, 100));
        PhysicsManager::GetSingleton().collideAABox(water_box, collector,
                                                    SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::MOVING),
                                                    SpecifiedObjectLayerFilter(PhysicsManager::Layers::MOVING));

        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }
};

}  // namespace

bool PhysicsWaterShapeApp::prepare(Platform& platform) {
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

void PhysicsWaterShapeApp::loadScene() {
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

        // Create scaled box
        BodyID body_id = body_interface
                                 .CreateBody(BodyCreationSettings(
                                         new ScaledShape(new BoxShape(Vec3(1.0f, 2.0f, 2.5f)), Vec3(0.5f, 0.6f, -0.7f)),
                                         Vec3(-10, 20, 0), Quat::sIdentity(), EMotionType::Dynamic,
                                         PhysicsManager::Layers::MOVING))
                                 ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create box
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(new BoxShape(Vec3(1.0f, 2.0f, 2.5f)), Vec3(-7, 20, 0),
                                                           Quat::sIdentity(), EMotionType::Dynamic,
                                                           PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create sphere
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(new SphereShape(2.0f), Vec3(-3, 20, 0), Quat::sIdentity(),
                                                           EMotionType::Dynamic, PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create static compound
        Ref<StaticCompoundShapeSettings> static_compound = new StaticCompoundShapeSettings;
        static_compound->AddShape(Vec3(2.0f, 0, 0), Quat::sIdentity(), new SphereShape(2.0f));
        static_compound->AddShape(Vec3(-1.0f, 0, 0), Quat::sIdentity(), new SphereShape(1.0f));

        body_id = body_interface
                          .CreateBody(BodyCreationSettings(static_compound, Vec3(3, 20, 0), Quat::sIdentity(),
                                                           EMotionType::Dynamic, PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create tetrahedron
        Array<Vec3> tetrahedron;
        tetrahedron.push_back(Vec3(-2, 0, -2));
        tetrahedron.push_back(Vec3(0, 0, 2));
        tetrahedron.push_back(Vec3(2, 0, -2));
        tetrahedron.push_back(Vec3(0, -2, 0));
        Ref<ConvexHullShapeSettings> tetrahedron_shape = new ConvexHullShapeSettings(tetrahedron);
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(tetrahedron_shape, Vec3(10, 20, 0), Quat::sIdentity(),
                                                           EMotionType::Dynamic, PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Non-uniform scaled tetrahedron
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(
                                  new ScaledShapeSettings(tetrahedron_shape, Vec3(1, -1.5f, 2.0f)), Vec3(15, 20, 0),
                                  Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create convex hull box
        Array<Vec3> box;
        box.push_back(Vec3(1.5f, 1.0f, 0.5f));
        box.push_back(Vec3(-1.5f, 1.0f, 0.5f));
        box.push_back(Vec3(1.5f, -1.0f, 0.5f));
        box.push_back(Vec3(-1.5f, -1.0f, 0.5f));
        box.push_back(Vec3(1.5f, 1.0f, -0.5f));
        box.push_back(Vec3(-1.5f, 1.0f, -0.5f));
        box.push_back(Vec3(1.5f, -1.0f, -0.5f));
        box.push_back(Vec3(-1.5f, -1.0f, -0.5f));
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(new ConvexHullShapeSettings(box), Vec3(18, 20, 0),
                                                           Quat::sIdentity(), EMotionType::Dynamic,
                                                           PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create random convex shape
        default_random_engine random;
        uniform_real_distribution<float> hull_size(0.1f, 1.9f);
        Array<Vec3> points;
        for (int j = 0; j < 20; ++j) points.push_back(hull_size(random) * Vec3::sRandom(random));
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(new ConvexHullShapeSettings(points), Vec3(21, 20, 0),
                                                           Quat::sIdentity(), EMotionType::Dynamic,
                                                           PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create mutable compound
        Ref<MutableCompoundShapeSettings> mutable_compound = new MutableCompoundShapeSettings;
        mutable_compound->AddShape(Vec3(1.0f, 0, 0), Quat::sIdentity(), new BoxShape(Vec3(0.5f, 0.75f, 1.0f)));
        mutable_compound->AddShape(Vec3(-1.0f, 0, 0), Quat::sIdentity(), new SphereShape(1.0f));

        body_id = body_interface
                          .CreateBody(BodyCreationSettings(mutable_compound, Vec3(25, 20, 0), Quat::sIdentity(),
                                                           EMotionType::Dynamic, PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);

        // Create box with center of mass offset
        body_id = body_interface
                          .CreateBody(BodyCreationSettings(
                                  new OffsetCenterOfMassShapeSettings(Vec3(-1.0f, 0.0f, 0.0f),
                                                                      new BoxShape(Vec3(2.0f, 0.25f, 0.25f))),
                                  Vec3(30, 20, 0), Quat::sIdentity(), EMotionType::Dynamic,
                                  PhysicsManager::Layers::MOVING))
                          ->GetID();
        body_interface.AddBody(body_id, EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
