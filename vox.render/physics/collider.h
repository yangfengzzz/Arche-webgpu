//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include "vox.math/matrix4x4.h"
#include "vox.render/component.h"
#include "vox.render/update_flag.h"

namespace vox {
class Collider : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Collider(Entity* entity);

    ~Collider() override;

    void setShape(std::unique_ptr<JPH::Shape>&& shape, JPH::EMotionType type);

    const JPH::Shape& getShape();

public:
    [[nodiscard]] Vector3F getCenterOfMassPosition() const;

    [[nodiscard]] Matrix4x4F getCenterOfMassTransform() const;

    /// Linear or angular velocity (functions will activate body if needed).
    /// Note that the linear velocity is the velocity of the center of mass, which may not coincide with the position of
    /// your object, to correct for this: \f$VelocityCOM = Velocity - AngularVelocity \times ShapeCOM\f$
    void setLinearAndAngularVelocity(const Vector3F& inLinearVelocity, const Vector3F& inAngularVelocity);

    void getLinearAndAngularVelocity(Vector3F& outLinearVelocity, Vector3F& outAngularVelocity) const;

    void setLinearVelocity(const Vector3F& inLinearVelocity);

    [[nodiscard]] Vector3F getLinearVelocity() const;

    ///< Add velocity to current velocity
    void addLinearVelocity(const Vector3F& inLinearVelocity);

    ///< Add linear and angular to current velocities
    void addLinearAndAngularVelocity(const Vector3F& inLinearVelocity, const Vector3F& inAngularVelocity);

    void setAngularVelocity(const Vector3F& inAngularVelocity);

    [[nodiscard]] Vector3F getAngularVelocity() const;

    ///< Velocity of point inPoint (in world space, e.g. on the surface of the body) of the body
    [[nodiscard]] Vector3F getPointVelocity(const Vector3F& inPoint) const;

public:
    /// Add forces to the body
    void addForce(const Vector3F& inForce);

    void addForce(const Vector3F& inForce, const Vector3F& inPoint);

    void addTorque(const Vector3F& inTorque);

    void addForceAndTorque(const Vector3F& inForce, const Vector3F& inTorque);

    /// Add an impulse to the body
    void addImpulse(const Vector3F& inImpulse);

    void addImpulse(const Vector3F& inImpulse, const Vector3F& inPoint);

    void addAngularImpulse(const Vector3F& inAngularImpulse);

public:
    void setMotionType(JPH::EMotionType inMotionType, JPH::EActivation inActivationMode);

    [[nodiscard]] JPH::EMotionType getMotionType() const;

    /// Get inverse inertia tensor in world space
    [[nodiscard]] Matrix4x4F getInverseInertia() const;

    void setRestitution(float inRestitution);

    [[nodiscard]] float getRestitution() const;

    void setFriction(float inFriction);

    [[nodiscard]] float getFriction() const;

    void setGravityFactor(float inGravityFactor);

    [[nodiscard]] float getGravityFactor() const;

public:
    void onUpdate();

    void onLateUpdate();

    void _onEnable() override;

    void _onDisable() override;

    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json &data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer &p_root) override;

private:
    std::unique_ptr<UpdateFlag> update_flag_;

    JPH::BodyInterface* _bodyInterface{nullptr};
    std::unique_ptr<JPH::Shape> _shape{nullptr};
    JPH::BodyID _bodyID;
    JPH::Body* _body{nullptr};
};
}  // namespace vox