//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
