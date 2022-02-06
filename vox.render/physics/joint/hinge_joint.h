//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef hinge_joint_hpp
#define hinge_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/**
 * A joint which behaves in a similar way to a hinge or axle.
 */
class HingeJoint : public Joint {
public:
    HingeJoint(Collider *collider0, Collider *collider1);
    
    float angle() const;
    
    float velocity() const;
    
    void setLimit(const PxJointAngularLimitPair &limits);
    
    PxJointAngularLimitPair limit() const;
    
    void setDriveVelocity(float velocity, bool autowake = true);
    
    float driveVelocity() const;
    
    void setDriveForceLimit(float limit);
    
    float driveForceLimit() const;
    
    void setDriveGearRatio(float ratio);
    
    float driveGearRatio() const;
    
    void setRevoluteJointFlags(PxRevoluteJointFlags flags);
    
    void setRevoluteJointFlag(PxRevoluteJointFlag::Enum flag, bool value);
    
    PxRevoluteJointFlags revoluteJointFlags() const;
    
    void setProjectionLinearTolerance(float tolerance);
    
    float projectionLinearTolerance() const;
    
    void setProjectionAngularTolerance(float tolerance);
    
    float projectionAngularTolerance() const;
};

}
}
#endif /* hinge_joint_hpp */
