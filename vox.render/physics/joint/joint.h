//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef joint_hpp
#define joint_hpp

#include "../physics.h"
#include "transform3.h"

namespace vox {
namespace physics {
/**
 * A base class providing common functionality for joints.
 */
class Joint {
public:
    void setActors(Collider *actor0, Collider *actor1);
    
    void setLocalPose(PxJointActorIndex::Enum actor, const Transform3F &localPose);
    
    Transform3F localPose(PxJointActorIndex::Enum actor) const;
    
    Transform3F relativeTransform() const;
    
    Vector3F relativeLinearVelocity() const;
    
    Vector3F relativeAngularVelocity() const;
    
    void setBreakForce(float force, float torque);
    
    void getBreakForce(float &force, float &torque) const;
    
    void setConstraintFlags(PxConstraintFlags flags);
    
    void setConstraintFlag(PxConstraintFlag::Enum flag, bool value);
    
    PxConstraintFlags constraintFlags() const;
    
    void setInvMassScale0(float invMassScale);
    
    float invMassScale0() const;
    
    void setInvInertiaScale0(float invInertiaScale);
    
    float invInertiaScale0() const;
    
    void setInvMassScale1(float invMassScale);
    
    float invMassScale1() const;
    
    void setInvInertiaScale1(float invInertiaScale);
    
    float invInertiaScale1() const;
    
protected:
    PxJoint *_nativeJoint;
};

}
}

#endif /* joint_hpp */
