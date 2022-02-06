//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef spring_joint_hpp
#define spring_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/**
 * A joint that maintains an upper or lower bound (or both) on the distance between two points on different objects.
 */
class SpringJoint : public Joint {
public:
    SpringJoint(Collider *collider0, Collider *collider1);
    
    float distance() const;
    
    void setMinDistance(float distance);
    
    float minDistance() const;
    
    void setMaxDistance(float distance);
    
    float maxDistance() const;
    
    void setTolerance(float tolerance);
    
    float tolerance() const;
    
    void setStiffness(float stiffness);
    
    float stiffness() const;
    
    void setDamping(float damping);
    
    float damping() const;
    
    void setDistanceJointFlags(PxDistanceJointFlags flags);
    
    void setDistanceJointFlag(PxDistanceJointFlag::Enum flag, bool value);
    
    PxDistanceJointFlags distanceJointFlags() const;
};

}
}
#endif /* spring_joint_hpp */
