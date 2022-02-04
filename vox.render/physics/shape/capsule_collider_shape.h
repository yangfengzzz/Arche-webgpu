//
//  capsule_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef capsule_collider_shape_hpp
#define capsule_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
/**
 * The up axis of the collider shape.
 */
struct ColliderShapeUpAxis {
    enum Enum {
        /** Up axis is X. */
        X,
        /** Up axis is Y. */
        Y,
        /** Up axis is Z. */
        Z
    };
};

/**
 * Physical collider shape for capsule.
 */
class CapsuleColliderShape : public ColliderShape {
public:
    CapsuleColliderShape();
    
    /**
     * Radius of capsule.
     */
    float radius();
    
    void setRadius(float value);
    
    /**
     * Height of capsule.
     */
    float height();
    
    void setHeight(float value);
    
    /**
     * Up axis of capsule.
     */
    ColliderShapeUpAxis::Enum upAxis();
    
    void setUpAxis(ColliderShapeUpAxis::Enum value);
    
    void setWorldScale(const Vector3F &scale) override;
    
private:
    float _radius = 1;
    float _height = 2;
    ColliderShapeUpAxis::Enum _upAxis = ColliderShapeUpAxis::Enum::Y;
};

}
}
#endif /* capsule_collider_shape_hpp */
