//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef entity_creation_menu_hpp
#define entity_creation_menu_hpp

#include <functional>
#include "ui/widgets/menu/menu_list.h"
#include "entity.h"

namespace vox {
namespace editor {
/**
 * Class exposing tools to generate an entity creation menu
 */
class EntityCreationMenu {
public:
    /**
     * Disabled constructor
     */
    EntityCreationMenu() = delete;
    
    /**
     * Generates an entity creation menu under the given MenuList item.
     * Also handles custom additionnal OnClick callback
     */
    static void generateEntityCreationMenu(::vox::ui::MenuList& menuList, EntityPtr p_parent = nullptr,
                                           std::optional<std::function<void()>> onItemClicked = {});
};

}
}
#endif /* entity_creation_menu_hpp */
