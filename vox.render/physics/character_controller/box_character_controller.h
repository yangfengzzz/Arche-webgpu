//
//  box_character_controller.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef box_character_controller_hpp
#define box_character_controller_hpp

#include "character_controller.h"

namespace vox {
namespace physics {
/**
 * A box character controller.
 */
class BoxCharacterController : public CharacterController {
public:
    BoxCharacterController(Entity *entity);
    
    void setDesc(const PxBoxControllerDesc &desc);
    
    float halfHeight() const;
    
    float halfSideExtent() const;
    
    float halfForwardExtent() const;
    
    bool setHalfHeight(float halfHeight);
    
    bool setHalfSideExtent(float halfSideExtent);
    
    bool setHalfForwardExtent(float halfForwardExtent);
};

}
}

#endif /* box_character_controller_hpp */
