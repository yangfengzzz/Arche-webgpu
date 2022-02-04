//
//  static_collider.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef static_collider_hpp
#define static_collider_hpp

#include "collider.h"

namespace vox {
namespace physics {
class StaticCollider : public Collider {
public:
    StaticCollider(Entity *entity);
};

}
}

#endif /* static_collider_hpp */
