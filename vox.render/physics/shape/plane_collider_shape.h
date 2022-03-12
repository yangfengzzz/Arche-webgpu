//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
};

}
}
#endif /* plane_collider_shape_hpp */
