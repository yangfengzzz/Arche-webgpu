//
//  capsule_character_controller.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef capsule_character_controller_hpp
#define capsule_character_controller_hpp

#include "character_controller.h"

namespace vox {
namespace physics {
/**
 * A capsule character controller.
 */
class CapsuleCharacterController : public CharacterController {
public:
    CapsuleCharacterController(Entity *entity);
    
    void setDesc(const PxCapsuleControllerDesc &desc);
    
    float radius() const;
    
    bool setRadius(float radius);
    
    float height() const;
    
    bool setHeight(float height);
    
    PxCapsuleClimbingMode::Enum climbingMode() const;
    
    bool setClimbingMode(PxCapsuleClimbingMode::Enum mode);
};

}
}
#endif /* capsule_character_controller_hpp */
