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
    Hierarchy(const std::string &p_title,
              bool p_opened,
              const PanelWindowSettings &p_windowSettings);
    
    /**
     * Clear hierarchy nodes
     */
    void clear();
    
    /**
     * Unselect every widgets
     */
    void unselectEntitiesWidgets();
    
    /**
     * Select the widget corresponding to the given entity
     * @param p_entity p_entity
     */
    void selectEntityByInstance(Entity* p_entity);
    
    /**
     * Select the widget
     * @param p_widget p_widget
     */
    void selectEntityByWidget(TreeNode &p_widget);
    
    /**
     * Attach the given entity linked widget to its parent widget
     * @param p_entity p_entity
     */
    void attachEntityToParent(Entity* p_entity);
    
    /**
     * Detach the given entity linked widget from its parent widget
     * @param p_entity p_entity
     */
    void detachFromParent(Entity* p_entity);
    
    /**
     * Delete the widget referencing the given entity
     * @param p_entity p_entity
     */
    void deleteEntityByInstance(Entity* p_entity);
    
    /**
     * Add a widget referencing the given entity
     * @param p_entity p_entity
     */
    void addEntityByInstance(Entity* p_entity);
    
public:
    Event<Entity* &> entitySelectedEvent;
    Event<Entity* &> entityUnselectedEvent;
    
private:
    TreeNode *_sceneRoot;
    
    std::unordered_map<Entity*, TreeNode *> _widgetEntityLink;
};

}
}
}
#endif /* hierarchy_hpp */
