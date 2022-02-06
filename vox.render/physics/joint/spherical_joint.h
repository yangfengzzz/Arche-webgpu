//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef spherical_joint_hpp
#define spherical_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/**
 * A joint which behaves in a similar way to a ball and socket.
 */
class SphericalJoint : public Joint {
public:
    SphericalJoint(Collider *collider0, Collider *collider1);
    
    PxJointLimitCone limitCone() const;
    
    void setLimitCone(const PxJointLimitCone &limit);
    
    float swingYAngle() const;
    
    float swingZAngle() const;
    
    void setSphericalJointFlags(PxSphericalJointFlags flags);
    
    void setSphericalJointFlag(PxSphericalJointFlag::Enum flag, bool value);
    
    PxSphericalJointFlags sphericalJointFlags() const;
    
    void setProjectionLinearTolerance(float tolerance);
    
    float projectionLinearTolerance() const;
};

}
}
#endif /* spherical_joint_hpp */
