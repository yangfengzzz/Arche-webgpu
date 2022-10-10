//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/hinge_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

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

bool PhysicsHingeConstraintApp::prepare(Platform& platform) {
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

void PhysicsHingeConstraintApp::loadScene() {
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

        float box_size = 4.0f;
        RefConst<Shape> box = new BoxShape(Vec3::sReplicate(0.5f * box_size));

        constexpr int cChainLength = 15;
        constexpr float cMinAngle = DegreesToRadians(-10.0f);
        constexpr float cMaxAngle = DegreesToRadians(20.0f);

        // Build a collision group filter that disables collision between adjacent bodies
        Ref<GroupFilterTable> group_filter = new GroupFilterTable(cChainLength);
        for (CollisionGroup::SubGroupID i = 0; i < cChainLength - 1; ++i) group_filter->DisableCollision(i, i + 1);

        // Bodies attached through hinge constraints
        for (int randomness = 0; randomness < 2; ++randomness) {
            CollisionGroup::GroupID group_id = CollisionGroup::GroupID(randomness);

            Vec3 position(0, 50, -randomness * 20.0f);
            Body& top = *body_interface.CreateBody(BodyCreationSettings(
                    box, position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
            top.SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(top.GetID(), EActivation::DontActivate);

            default_random_engine random;
            uniform_real_distribution<float> displacement(-1.0f, 1.0f);

            Body* prev = &top;
            for (int i = 1; i < cChainLength; ++i) {
                Quat rotation;
                if (randomness == 0) {
                    position += Vec3(box_size, 0, 0);
                    rotation = Quat::sIdentity();
                } else {
                    position += Vec3(box_size + abs(displacement(random)), displacement(random), displacement(random));
                    rotation = Quat::sRandom(random);
                }

                Body& segment = *body_interface.CreateBody(BodyCreationSettings(
                        box, position, rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                segment.SetCollisionGroup(CollisionGroup(group_filter, group_id, CollisionGroup::SubGroupID(i)));
                body_interface.AddBody(segment.GetID(), EActivation::Activate);

                HingeConstraintSettings settings;
                if ((i & 1) == 0) {
                    settings.mPoint1 = settings.mPoint2 = position + Vec3(-0.5f * box_size, 0, 0.5f * box_size);
                    settings.mHingeAxis1 = settings.mHingeAxis2 = Vec3::sAxisY();
                    settings.mNormalAxis1 = settings.mNormalAxis2 = Vec3::sAxisX();
                } else {
                    settings.mPoint1 = settings.mPoint2 = position + Vec3(-0.5f * box_size, -0.5f * box_size, 0);
                    settings.mHingeAxis1 = settings.mHingeAxis2 = Vec3::sAxisZ();
                    settings.mNormalAxis1 = settings.mNormalAxis2 = Vec3::sAxisX();
                }
                settings.mLimitsMin = cMinAngle;
                settings.mLimitsMax = cMaxAngle;
                PhysicsManager::GetSingleton().addConstraint(settings.Create(*prev, segment));

                prev = &segment;
            }
        }
    }
    scene->play();
}

}  // namespace vox
