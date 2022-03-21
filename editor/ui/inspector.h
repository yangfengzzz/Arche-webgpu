//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef inspector_hpp
#define inspector_hpp

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/buttons/button.h"
#include "ui/widgets/selection/combo_box.h"

#include "hierarchy.h"
#include "asset_browser.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class Inspector : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    Inspector(const std::string &p_title,
              bool p_opened,
              const PanelWindowSettings &p_windowSettings);
    
    /**
     * Destructor
     */
    ~Inspector();
    
    /**
     * Focus the given actor
     * @param p_target p_target
     */
    void focusActor(EntityPtr &p_target);
    
    /**
     * Unfocus the currently targeted actor
     */
    void unFocus();
    
    /**
     * Unfocus the currently targeted actor without removing listeners attached to this actor
     */
    void softUnFocus();
    
    /**
     * Returns the currently selected actor
     */
    EntityPtr targetActor() const;
    
    /**
     * Create the actor inspector for the given actor
     */
    void createActorInspector(EntityPtr &p_target);
    
    /**
     * Draw the given component in inspector
     */
    void drawComponent(Component *p_component);
    
    /**
     * Draw the given behaviour in inspector
     */
    //    void drawBehaviour(OvCore::ECS::Components::Behaviour& p_behaviour);
    
    /**
     * Refresh the inspector
     */
    void refresh();
    
private:
    EntityPtr _targetActor{nullptr};
    Group *_actorInfo;
    Group *_inspectorHeader;
    ComboBox *_componentSelectorWidget;
    InputText *_scriptSelectorWidget;
    
    uint64_t _componentAddedListener = 0;
    uint64_t _componentRemovedListener = 0;
    uint64_t _behaviourAddedListener = 0;
    uint64_t _behaviourRemovedListener = 0;
    uint64_t _destroyedListener = 0;
};

}
}
}
#endif /* inspector_hpp */
