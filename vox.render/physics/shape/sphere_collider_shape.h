//
//  sphere_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

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
    
private:
    float _radius = 1.0;
};

}
}

#endif /* sphere_collider_shape_hpp */
