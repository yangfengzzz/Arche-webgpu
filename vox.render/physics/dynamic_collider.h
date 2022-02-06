//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef dynamic_collider_hpp
#define dynamic_collider_hpp

#include "collider.h"
#include "transform3.h"

namespace vox {
namespace physics {
class DynamicCollider : public Collider {
public:
    DynamicCollider(Entity *entity);
    
    /**
     * The linear damping of the dynamic collider.
     */
    float linearDamping();
    
    void setLinearDamping(float newValue);
    
    /**
     * The angular damping of the dynamic collider.
     */
    float angularDamping();
    
    void setAngularDamping(float newValue);
    
    /**
     * The linear velocity vector of the dynamic collider measured in world unit per second.
     */
    Vector3F linearVelocity();
    
    void setLinearVelocity(const Vector3F &newValue);
    
    /**
     * The angular velocity vector of the dynamic collider measured in radians per second.
     */
    Vector3F angularVelocity();
    
    void setAngularVelocity(const Vector3F &newValue);
    
    /**
     * The mass of the dynamic collider.
     */
    float mass();
    
    void setMass(float newValue);
    
    /**
     * The center of mass relative to the transform's origin.
     */
    Transform3F centerOfMass();
    
    void setCenterOfMass(const Transform3F &newValue);
    
    /**
     * The diagonal inertia tensor of mass relative to the center of mass.
     */
    Vector3F inertiaTensor();
    
    void setInertiaTensor(const Vector3F &newValue);
    
    /**
     * The maximum angular velocity of the collider measured in radians per second. (Default 7) range { 0, infinity }.
     */
    float maxAngularVelocity();
    
    void setMaxAngularVelocity(float newValue);
    
    /**
     * Maximum velocity of a collider when moving out of penetrating state.
     */
    float maxDepenetrationVelocity();
    
    void setMaxDepenetrationVelocity(float newValue);
    
    /**
     * The mass-normalized energy threshold, below which objects start going to sleep.
     */
    float sleepThreshold();
    
    void setSleepThreshold(float newValue);
    
    /**
     * The solverIterations determines how accurately collider joints and collision contacts are resolved.
     */
    uint32_t solverIterations();
    
    void setSolverIterations(uint32_t newValue);
    
public:
    /**
     * Controls whether physics affects the dynamic collider.
     */
    bool isKinematic();
    
    void setIsKinematic(bool newValue);
    
    PxRigidBodyFlags rigidBodyFlags() const;
    
    void setRigidBodyFlag(PxRigidBodyFlag::Enum flag, bool value);
    
    void setRigidBodyFlags(PxRigidBodyFlags inFlags);
    
public:
    /**
     * Controls whether physics will change the rotation of the object.
     */
    bool freezeRotation();
    
    void setFreezeRotation(bool newValue);
    
    /**
     * The particular rigid dynamic lock flag.
     */
    PxRigidDynamicLockFlags rigidDynamicLockFlags() const;
    
    void setRigidDynamicLockFlag(PxRigidDynamicLockFlag::Enum flag, bool value);
    
    void setRigidDynamicLockFlags(PxRigidDynamicLockFlags flags);
    
public:
    /**
     * Apply a force to the DynamicCollider.
     * @param force - The force make the collider move
     */
    void applyForce(const Vector3F &force);
    
    /**
     * Apply a torque to the DynamicCollider.
     * @param torque - The force make the collider rotate
     */
    void applyTorque(const Vector3F &torque);
    
    /**
     * Moves kinematically controlled dynamic actors through the game world.
     * @param pose The desired pose for the kinematic actor
     */
    void setKinematicTarget(const Transform3F &pose);
    
    /**
     * Forces a collider to sleep at least one frame.
     */
    void putToSleep();
    
    /**
     * Forces a collider to wake up.
     */
    void wakeUp();
    
private:
    void _onLateUpdate() override;
};

}
}

#endif /* dynamic_collider_hpp */
