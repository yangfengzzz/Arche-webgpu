//
//  plane_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef plane_collider_shape_hpp
#define plane_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
/**
 * Physical collider shape plane.
 */
class PlaneColliderShape : public ColliderShape {
public:
    PlaneColliderShape();
    
    /**
     * The local rotation of this plane.
     */
    Vector3F rotation();
    
    void setRotation(const Vector3F &value);
    
    void setWorldScale(const Vector3F &scale) {
    }
};

}
}
#endif /* plane_collider_shape_hpp */
