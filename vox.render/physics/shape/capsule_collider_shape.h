//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef capsule_collider_shape_hpp
#define capsule_collider_shape_hpp

#include "collider_shape.h"

#ifdef _DEBUG
#include "mesh/mesh_renderer.h"
#endif

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
    
#ifdef _DEBUG
    void setEntity(EntityPtr value) override;
    
    void _syncCapsuleGeometry();
    
    void _syncCapsuleAxis(ColliderShapeUpAxis::Enum upAxis);
    
    MeshRenderer* _renderer{nullptr};
#endif
    
private:
    float _radius = 1;
    float _height = 2;
    ColliderShapeUpAxis::Enum _upAxis = ColliderShapeUpAxis::Enum::Y;
};

}
}
#endif /* capsule_collider_shape_hpp */
