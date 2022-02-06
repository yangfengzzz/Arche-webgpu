//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef configurable_joint_hpp
#define configurable_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/**
 * A Configurable joint is a general constraint between two actors.
 */
class ConfigurableJoint : public Joint {
public:
    ConfigurableJoint(Collider *collider0, Collider *collider1);
    
    void setMotion(PxD6Axis::Enum axis, PxD6Motion::Enum type);
    
    PxD6Motion::Enum motion(PxD6Axis::Enum axis) const;
    
    float twistAngle() const;
    
    float swingYAngle() const;
    
    float swingZAngle() const;
    
public:
    void setDistanceLimit(const PxJointLinearLimit &limit);
    
    PxJointLinearLimit distanceLimit() const;
    
    void setLinearLimit(PxD6Axis::Enum axis, const PxJointLinearLimitPair &limit);
    
    PxJointLinearLimitPair linearLimit(PxD6Axis::Enum axis) const;
    
    void setTwistLimit(const PxJointAngularLimitPair &limit);
    
    PxJointAngularLimitPair twistLimit() const;
    
    void setSwingLimit(const PxJointLimitCone &limit);
    
    PxJointLimitCone swingLimit() const;
    
    void pyramidSwingLimit(const PxJointLimitPyramid &limit);
    
    PxJointLimitPyramid pyramidSwingLimit() const;
    
public:
    void setDrive(PxD6Drive::Enum index, const PxD6JointDrive &drive);
    
    PxD6JointDrive drive(PxD6Drive::Enum index) const;
    
    void setDrivePosition(const Transform3F &pose, bool autowake = true);
    
    Transform3F drivePosition() const;
    
    void setDriveVelocity(const Vector3F &linear, const Vector3F &angular, bool autowake = true);
    
    void driveVelocity(Vector3F &linear, Vector3F &angular) const;
    
    void setProjectionLinearTolerance(float tolerance);
    
    float projectionLinearTolerance() const;
    
    void setProjectionAngularTolerance(float tolerance);
    
    float projectionAngularTolerance() const;
};
}
}
#endif /* configurable_joint_hpp */
