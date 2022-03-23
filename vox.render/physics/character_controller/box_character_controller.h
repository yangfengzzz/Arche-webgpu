//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    BoxCharacterController(Entity *entity);
    
    void setDesc(const PxBoxControllerDesc &desc);
    
    float halfHeight() const;
    
    float halfSideExtent() const;
    
    float halfForwardExtent() const;
    
    bool setHalfHeight(float halfHeight);
    
    bool setHalfSideExtent(float halfSideExtent);
    
    bool setHalfForwardExtent(float halfForwardExtent);
    
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

#endif /* box_character_controller_hpp */
