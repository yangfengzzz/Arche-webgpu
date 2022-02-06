//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef translational_joint_hpp
#define translational_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/**
 * A translational joint permits relative translational movement between two bodies along
 * an axis, but no relative rotational movement.
 */
class TranslationalJoint : public Joint {
public:
    TranslationalJoint(Collider *collider0, Collider *collider1);
    
    float position() const;
    
    float velocity() const;
    
    void setLimit(const PxJointLinearLimitPair &);
    
    PxJointLinearLimitPair limit() const;
    
    void setPrismaticJointFlags(PxPrismaticJointFlags flags);
    
    void setPrismaticJointFlag(PxPrismaticJointFlag::Enum flag, bool value);
    
    PxPrismaticJointFlags translationalJointFlags() const;
    
    void setProjectionLinearTolerance(float tolerance);
    
    float projectionLinearTolerance() const;
    
    void setProjectionAngularTolerance(float tolerance);
    
    float projectionAngularTolerance() const;
};

}
}

#endif /* translational_joint_hpp */
