//
//  box_collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

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
    
private:
    Vector3F _half = Vector3F(0.5, 0.5, 0.5);
};

}
}
#endif /* box_collider_shape_hpp */
