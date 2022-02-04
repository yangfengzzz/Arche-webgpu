//
//  fixed_joint.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

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
