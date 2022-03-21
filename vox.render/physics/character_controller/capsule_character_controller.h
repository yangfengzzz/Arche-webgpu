//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
    
public:
    /**
     * Serialize the component
     */
    void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Deserialize the component
     */
    void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;
};

}
}
#endif /* capsule_character_controller_hpp */
