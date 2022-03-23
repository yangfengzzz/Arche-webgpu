//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hierarchy.h"

#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/visual/separator.h"
#include "ui/plugins/drag_drop_target.h"
#include "ui/plugins/drag_drop_source.h"
#include "ui/plugins/contextual_menu.h"
#include "editor_actions.h"
#include "entity_creation_menu.h"

namespace vox {
namespace editor {
namespace ui {
namespace {
class HierarchyContextualMenu : public ContextualMenu
{
public:
    HierarchyContextualMenu(EntityPtr p_target, TreeNode& p_treeNode, bool p_panelMenu = false) :
    _target(p_target),
    _treeNode(p_treeNode) {
        if (_target) {
            auto& focusButton = createWidget<MenuItem>("Focus");
            focusButton.clickedEvent += [this]
            {
                EditorActions::getSingleton().moveToTarget(_target);
            };
            
            auto& duplicateButton = createWidget<MenuItem>("Duplicate");
            duplicateButton.clickedEvent += [this]
            {
                EditorActions::getSingleton().delayAction(std::bind(&EditorActions::duplicateEntity,
                                                                    EditorActions::getSingletonPtr(), _target, nullptr, true), 0);
            };
            
            auto& deleteButton = createWidget<MenuItem>("Delete");
            deleteButton.clickedEvent += [this]
            {
                EditorActions::getSingleton().destroyEntity(_target);
            };
        }
        
        auto& createActor = createWidget<MenuList>("Create...");
        EntityCreationMenu::generateEntityCreationMenu(createActor, _target, std::bind(&TreeNode::open, &_treeNode));
    }
    
    virtual void execute() override {
        if (_widgets.size() > 0)
            ContextualMenu::execute();
    }
    
private:
    EntityPtr _target;
    TreeNode& _treeNode;
};

} // namespace

}
}
}
