//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef box_collider_shape_hpp
#define box_collider_shape_hpp

#include "collider_shape.h"

namespace vox {
namespace physics {
class BoxColliderShape : public ColliderShape {
public:
    BoxColliderShape();
    
    Vector3F size();
    
    void setSize(const Vector3F &value);
    
    void setWorldScale(const Vector3F &scale) override;
    
#ifdef _DEBUG
    void setEntity(Entity* value) override;
    
    void _syncBoxGeometry();
#endif
    
private:
    Vector3F _half = Vector3F(0.5, 0.5, 0.5);
};

}
}
#endif /* box_collider_shape_hpp */
