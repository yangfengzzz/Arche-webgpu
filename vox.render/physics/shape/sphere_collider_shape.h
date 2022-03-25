//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sphere_collider_shape_hpp
#define sphere_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
/**
 * Physical collider shape for sphere.
 */
class SphereColliderShape : public ColliderShape {
public:
    SphereColliderShape();
    
    /**
     * Radius of sphere shape.
     */
    float radius();
    
    void setRadius(float value);
    
    void setWorldScale(const Vector3F &scale) override;
    
#ifdef _DEBUG
    void setEntity(Entity* value) override;
    
    void _syncSphereGeometry();
#endif
    
private:
    float _radius = 1.0;
};

}
}

#endif /* sphere_collider_shape_hpp */
