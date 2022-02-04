//
//  spherical_joint.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

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
