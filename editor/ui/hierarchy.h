//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef hierarchy_hpp
#define hierarchy_hpp

#include "scene_forward.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/tree_node.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class Hierarchy : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    Hierarchy(const std::string& p_title,
              bool p_opened,
              const PanelWindowSettings& p_windowSettings);
    
    /**
     * Clear hierarchy nodes
     */
    void clear();
    
    /**
     * Unselect every widgets
     */
    void unselectActorsWidgets();
    
    /**
     * Select the widget corresponding to the given actor
     * @param p_actor p_actor
     */
    void selectActorByInstance(EntityPtr& p_actor);
    
    /**
     * Select the widget
     * @param p_actor p_actor
     */
    void selectActorByWidget(TreeNode& p_widget);
    
    /**
     * Attach the given actor linked widget to its parent widget
     * @param p_actor p_actor
     */
    void attachActorToParent(EntityPtr& p_actor);
    
    /**
     * Detach the given actor linked widget from its parent widget
     * @param p_actor p_actor
     */
    void detachFromParent(EntityPtr& p_actor);
    
    /**
     * Delete the widget referencing the given actor
     * @param p_actor p_actor
     */
    void deleteActorByInstance(EntityPtr& p_actor);
    
    /**
     * Add a widget referencing the given actor
     * @param p_actor p_actor
     */
    void AddActorByInstance(EntityPtr& p_actor);
    
public:
    Event<EntityPtr&> actorSelectedEvent;
    Event<EntityPtr&> actorUnselectedEvent;
    
private:
    TreeNode* m_sceneRoot;
    
    std::unordered_map<EntityPtr, TreeNode*> _widgetActorLink;
};

}
}
}
#endif /* hierarchy_hpp */
