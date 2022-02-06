//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef fixed_joint_hpp
#define fixed_joint_hpp

#include "joint.h"

namespace vox {
namespace physics {
/*
 * A fixed joint permits no relative movement between two bodies. ie the bodies are glued together.
 */
class FixedJoint : public Joint {
public:
    FixedJoint(Collider *collider0, Collider *collider1);
    
    void setProjectionLinearTolerance(float tolerance);
    
    float projectionLinearTolerance() const;
    
    void setProjectionAngularTolerance(float tolerance);
    
    float projectionAngularTolerance() const;
};

}
}

#endif /* fixed_joint_hpp */
