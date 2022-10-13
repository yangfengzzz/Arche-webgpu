//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/high_speed_app.h"

#include <Jolt/ObjectStream/ObjectStreamIn.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/PhysicsScene.h>

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

bool PhysicsHighSpeedApp::prepare(Platform &platform) {
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

const char *PhysicsHighSpeedApp::sScenes[] = {
        "Simple",
        "Convex Hull On Large Triangles",
        "Convex Hull On Terrain1",
};

int PhysicsHighSpeedApp::sSelectedScene = 0;

void PhysicsHighSpeedApp::loadScene() {
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
        switch (sSelectedScene) {
            case 0:
                createSimpleScene();
                break;

            case 1:
                createConvexOnLargeTriangles();
                break;

            case 2:
                createConvexOnTerrain1();
                break;

            default:
                JPH_ASSERT(false);
                break;
        }
    }
    scene->play();
}

void PhysicsHighSpeedApp::createDominoBlocks(Vec3Arg inOffset, int inNumWalls, float inDensity, float inRadius) {
    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();

    BodyCreationSettings box_settings;
    Ref<BoxShape> box_shape = new BoxShape(Vec3(0.9f, 1.0f, 0.1f));
    box_shape->SetDensity(inDensity);  // Make box more heavy so the bouncing ball keeps a higher velocity
    box_settings.SetShape(box_shape);
    box_settings.mObjectLayer = PhysicsManager::Layers::MOVING;

    // U shaped set of thin boxes
    for (int i = 0; i < inNumWalls; ++i) {
        box_settings.mPosition = inOffset + Vec3(2.0f * i, 1, -1.1f - inRadius);
        body_interface.CreateAndAddBody(box_settings, EActivation::DontActivate);

        box_settings.mPosition = inOffset + Vec3(2.0f * i, 1, +1.1f + inRadius);
        body_interface.CreateAndAddBody(box_settings, EActivation::DontActivate);
    }

    box_settings.mPosition = inOffset + Vec3(-1.1f - inRadius, 1, 0);
    box_settings.mRotation = Quat::sRotation(Vec3::sAxisY(), 0.5f * JPH_PI);
    body_interface.CreateAndAddBody(box_settings, EActivation::DontActivate);
}

void PhysicsHighSpeedApp::createDynamicObject(Vec3 inPosition,
                                              Vec3 inVelocity,
                                              Shape *inShape,
                                              EMotionQuality inMotionQuality) {
    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();

    BodyCreationSettings creation_settings;
    creation_settings.SetShape(inShape);
    creation_settings.mFriction = 0.0f;
    creation_settings.mRestitution = 1.0f;
    creation_settings.mLinearDamping = 0.0f;
    creation_settings.mAngularDamping = 0.0f;
    creation_settings.mMotionQuality = inMotionQuality;
    creation_settings.mObjectLayer = PhysicsManager::Layers::MOVING;
    creation_settings.mPosition = inPosition;

    Body &body = *body_interface.CreateBody(creation_settings);
    body.SetLinearVelocity(inVelocity);
    body_interface.AddBody(body.GetID(), inVelocity.IsNearZero() ? EActivation::DontActivate : EActivation::Activate);
}

void PhysicsHighSpeedApp::createSimpleScene() {
    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();

    // Floor
    PhysicsUtils::createFloor(body_interface);

    const float radius = 0.1f;
    const int num_walls = 5;
    const float density = 2000.0f;
    const float speed = 240.0f;

    Vec3 offset(0, 0, -30);

    {
        // U shaped set of thin walls
        TriangleList triangles;
        for (int i = 0; i < num_walls; ++i) {
            triangles.push_back(Triangle(Float3(2.0f * i - 1, 0, -1 - radius), Float3(2.0f * i + 1, 0, -1 - radius),
                                         Float3(2.0f * i, 2, -1 - radius)));
            triangles.push_back(Triangle(Float3(2.0f * i - 1, 0, 1 + radius), Float3(2.0f * i, 2, 1 + radius),
                                         Float3(2.0f * i + 1, 0, 1 + radius)));
        }
        triangles.push_back(Triangle(Float3(-1 - radius, 0, -1), Float3(-1 - radius, 2, 0), Float3(-1 - radius, 0, 1)));
        Body &walls = *body_interface.CreateBody(BodyCreationSettings(new MeshShapeSettings(triangles), offset,
                                                                      Quat::sIdentity(), EMotionType::Static,
                                                                      PhysicsManager::Layers::NON_MOVING));
        walls.SetRestitution(1.0f);
        walls.SetFriction(0.0f);
        body_interface.AddBody(walls.GetID(), EActivation::DontActivate);

        // Fast moving sphere against mesh
        createDynamicObject(offset + Vec3(2.0f * num_walls - 1, 1, 0), Vec3(-speed, 0, -speed),
                            new SphereShape(radius));
    }

    offset += Vec3(0, 0, 5);

    {
        // Create wall of domino blocks
        createDominoBlocks(offset, num_walls, density, radius);

        // Fast moving sphere against domino blocks
        createDynamicObject(offset + Vec3(2.0f * num_walls - 1, 1, 0), Vec3(-speed, 0, -speed),
                            new SphereShape(radius));
    }

    offset += Vec3(0, 0, 5);

    {
        // Create wall of domino blocks
        createDominoBlocks(offset, num_walls, density, radius);

        // Fast moving scaled box against domino blocks
        createDynamicObject(
                offset + Vec3(2.0f * num_walls - 1, 1, 0), Vec3(-speed, 0, -speed),
                new ScaledShape(new BoxShape(Vec3::sReplicate(0.5f * radius), 0.01f), Vec3::sReplicate(2.0f)));
    }

    offset += Vec3(0, 0, 5);

    {
        // Fast moving box stuck in ground moving, one moving up, one moving down
        createDynamicObject(offset + Vec3(-1, 0, 0), Vec3(0, speed, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(1, 0, 0), Vec3(0, -speed, 0), new BoxShape(Vec3::sReplicate(radius)));
    }

    offset += Vec3(0, 0, 5);

    {
        // Single shape that has 4 walls to surround fast moving sphere
        BodyCreationSettings enclosing_settings;
        Ref<BoxShapeSettings> box_shape = new BoxShapeSettings(Vec3(1.0f, 1.0f, 0.1f));
        Ref<StaticCompoundShapeSettings> enclosing_shape = new StaticCompoundShapeSettings();
        enclosing_shape->AddShape(Vec3(0, 0, 1), Quat::sIdentity(), box_shape);
        enclosing_shape->AddShape(Vec3(0, 0, -1), Quat::sIdentity(), box_shape);
        enclosing_shape->AddShape(Vec3(1, 0, 0), Quat::sRotation(Vec3::sAxisY(), 0.5f * JPH_PI), box_shape);
        enclosing_shape->AddShape(Vec3(-1, 0, 0), Quat::sRotation(Vec3::sAxisY(), 0.5f * JPH_PI), box_shape);
        enclosing_settings.SetShapeSettings(enclosing_shape);
        enclosing_settings.mMotionType = EMotionType::Kinematic;
        enclosing_settings.mObjectLayer = PhysicsManager::Layers::MOVING;
        enclosing_settings.mPosition = offset + Vec3(0, 1, 0);
        Body &enclosing = *body_interface.CreateBody(enclosing_settings);
        body_interface.AddBody(enclosing.GetID(), EActivation::Activate);

        // Fast moving sphere in box
        createDynamicObject(offset + Vec3(0, 0.5f, 0), Vec3(-speed, 0, -0.5f * speed), new SphereShape(radius));
    }

    offset += Vec3(0, 0, 5);

    {
        // Two boxes on a collision course
        createDynamicObject(offset + Vec3(1, 0.5f, 0), Vec3(-speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(-1, 0.5f, 0), Vec3(speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
    }

    offset += Vec3(0, 0, 5);

    {
        // Two boxes on a collision course, off center
        createDynamicObject(offset + Vec3(1, 0.5f, 0), Vec3(-speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(-1, 0.5f, radius), Vec3(speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
    }

    offset += Vec3(0, 0, 5);

    {
        // Two boxes on a collision course, one discrete
        createDynamicObject(offset + Vec3(1, 0.5f, 0), Vec3(-speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(-1, 0.5f, 0), Vec3(60.0f, 0, 0), new BoxShape(Vec3::sReplicate(radius)),
                            EMotionQuality::Discrete);
    }

    offset += Vec3(0, 0, 5);

    {
        // Two boxes on a collision course, one inactive
        createDynamicObject(offset + Vec3(1, 0.5f, 0), Vec3(-speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(0, 0.5f, 0), Vec3::sZero(), new BoxShape(Vec3::sReplicate(radius)));
    }

    offset += Vec3(0, 0, 5);

    {
        // Two boxes on a collision course, one inactive and discrete
        createDynamicObject(offset + Vec3(1, 0.5f, 0), Vec3(-speed, 0, 0), new BoxShape(Vec3::sReplicate(radius)));
        createDynamicObject(offset + Vec3(0, 0.5f, 0), Vec3::sZero(), new BoxShape(Vec3::sReplicate(radius)),
                            EMotionQuality::Discrete);
    }

    offset += Vec3(0, 0, 5);

    {
        // Create long thin shape
        BoxShapeSettings box_settings(Vec3(0.05f, 0.8f, 0.03f), 0.015f);
        box_settings.SetEmbedded();
        BodyCreationSettings body_settings(&box_settings, offset + Vec3(0, 1, 0),
                                           Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI), EMotionType::Dynamic,
                                           PhysicsManager::Layers::MOVING);
        body_settings.mMotionQuality = EMotionQuality::LinearCast;
        body_settings.mRestitution = 0.0f;
        body_settings.mFriction = 1.0f;

        Body &body = *body_interface.CreateBody(body_settings);
        body.SetLinearVelocity(Vec3(0, -100.0f, 0));
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }

    offset += Vec3(0, 0, 5);

    {
        // Create long thin shape under 45 degrees
        BoxShapeSettings box_settings(Vec3(0.05f, 0.8f, 0.03f), 0.015f);
        box_settings.SetEmbedded();
        BodyCreationSettings body_settings(&box_settings, offset + Vec3(0, 1, 0),
                                           Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Dynamic,
                                           PhysicsManager::Layers::MOVING);
        body_settings.mMotionQuality = EMotionQuality::LinearCast;
        body_settings.mRestitution = 0.0f;
        body_settings.mFriction = 1.0f;

        Body &body = *body_interface.CreateBody(body_settings);
        body.SetLinearVelocity(Vec3(0, -100.0f, 0));
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }

    offset += Vec3(0, 0, 5);

    {
        // Create long thin shape with restitution
        BoxShapeSettings box_settings(Vec3(0.05f, 0.8f, 0.03f), 0.015f);
        box_settings.SetEmbedded();
        BodyCreationSettings body_settings(&box_settings, offset + Vec3(0, 1, 0),
                                           Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI), EMotionType::Dynamic,
                                           PhysicsManager::Layers::MOVING);
        body_settings.mMotionQuality = EMotionQuality::LinearCast;
        body_settings.mRestitution = 1.0f;
        body_settings.mFriction = 1.0f;

        Body &body = *body_interface.CreateBody(body_settings);
        body.SetLinearVelocity(Vec3(0, -100.0f, 0));
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }

    offset += Vec3(0, 0, 5);

    {
        // Create long thin shape under 45 degrees with restitution
        BoxShapeSettings box_settings(Vec3(0.05f, 0.8f, 0.03f), 0.015f);
        box_settings.SetEmbedded();
        BodyCreationSettings body_settings(&box_settings, offset + Vec3(0, 1, 0),
                                           Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Dynamic,
                                           PhysicsManager::Layers::MOVING);
        body_settings.mMotionQuality = EMotionQuality::LinearCast;
        body_settings.mRestitution = 1.0f;
        body_settings.mFriction = 1.0f;

        Body &body = *body_interface.CreateBody(body_settings);
        body.SetLinearVelocity(Vec3(0, -100.0f, 0));
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }
}

void PhysicsHighSpeedApp::createFastSmallConvexObjects() {
    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();

    // Create small convex hull
    Array<Vec3> vertices = {Vec3(-0.044661f, 0.001230f, 0.003877f),   Vec3(-0.024743f, -0.042562f, 0.003877f),
                            Vec3(-0.012336f, -0.021073f, 0.048484f),  Vec3(0.016066f, 0.028121f, -0.049904f),
                            Vec3(-0.023734f, 0.043275f, -0.024153f),  Vec3(0.020812f, 0.036341f, -0.019530f),
                            Vec3(0.012495f, 0.021936f, 0.045288f),    Vec3(0.026750f, 0.001230f, 0.049273f),
                            Vec3(0.045495f, 0.001230f, -0.022077f),   Vec3(0.022193f, -0.036274f, -0.021126f),
                            Vec3(0.022781f, -0.037291f, 0.029558f),   Vec3(0.014691f, -0.023280f, 0.052897f),
                            Vec3(-0.012187f, -0.020815f, -0.040214f), Vec3(0.000541f, 0.001230f, -0.056224f),
                            Vec3(-0.039882f, 0.001230f, -0.019461f),  Vec3(0.000541f, 0.001230f, 0.056022f),
                            Vec3(-0.020614f, -0.035411f, -0.020551f), Vec3(-0.019485f, 0.035916f, 0.027001f),
                            Vec3(-0.023968f, 0.043680f, 0.003877f),   Vec3(-0.020051f, 0.001230f, 0.039543f),
                            Vec3(0.026213f, 0.001230f, -0.040589f),   Vec3(-0.010797f, 0.020868f, 0.043152f),
                            Vec3(-0.012378f, 0.023607f, -0.040876f)};
    ConvexHullShapeSettings convex_settings(vertices);
    convex_settings.SetEmbedded();
    BodyCreationSettings body_settings(&convex_settings, Vec3::sZero(), Quat::sIdentity(), EMotionType::Dynamic,
                                       PhysicsManager::Layers::MOVING);
    body_settings.mMotionQuality = EMotionQuality::LinearCast;

    // Create many instances with high velocity
    default_random_engine rnd;
    uniform_real_distribution<float> restitution_distrib(0.0f, 0.1f);
    uniform_real_distribution<float> velocity_distrib(-10.0f, 10.0f);
    for (int x = -25; x < 25; ++x)
        for (int y = -25; y < 25; ++y) {
            // Cast a ray to find the terrain
            Vec3 origin(float(x), 100.0f, float(y));
            Vec3 direction(0, -100.0f, 0);
            RayCastResult hit;
            if (PhysicsManager::GetSingleton().castRay(
                        {origin, direction}, hit, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::NON_MOVING),
                        SpecifiedObjectLayerFilter(PhysicsManager::Layers::NON_MOVING))) {
                // Place 10m above terrain
                body_settings.mPosition = origin + hit.mFraction * direction + Vec3(0, 10.0f, 0);
                body_settings.mRotation = Quat::sRandom(rnd);
                body_settings.mRestitution = restitution_distrib(rnd);

                Body &body = *body_interface.CreateBody(body_settings);
                body.SetLinearVelocity(Vec3(velocity_distrib(rnd), -100.0f, velocity_distrib(rnd)));
                body_interface.AddBody(body.GetID(), EActivation::Activate);
            }
        }
}

void PhysicsHighSpeedApp::createConvexOnLargeTriangles() {
    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();

    // Create floor
    PhysicsUtils::createLargeTriangleFloor(body_interface);

    createFastSmallConvexObjects();
}

void PhysicsHighSpeedApp::createConvexOnTerrain1() {
    // Load scene
    //    Ref<PhysicsScene> scene;
    //    if (!ObjectStreamIn::sReadObject("Assets/terrain1.bof", scene)) FatalError("Failed to load scene");
    //    for (BodyCreationSettings &body : scene->GetBodies()) body.mObjectLayer = PhysicsManager::Layers::NON_MOVING;
    //    scene->FixInvalidScales();
    //    scene->CreateBodies(mPhysicsSystem);
    //
    //    createFastSmallConvexObjects();
}

}  // namespace vox
