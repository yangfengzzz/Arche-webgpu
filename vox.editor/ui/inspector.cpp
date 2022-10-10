//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/inspector.h"

#include "vox.editor/editor_actions.h"
#include "vox.render/behaviour.h"
#include "vox.render/entity.h"
#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/plugins/drag_drop_target.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/drags/drag_float.h"
#include "vox.render/ui/widgets/drags/drag_multiple_floats.h"
#include "vox.render/ui/widgets/input_fields/input_float.h"
#include "vox.render/ui/widgets/layout/columns.h"
#include "vox.render/ui/widgets/layout/spacing.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/texts/text.h"
#include "vox.render/ui/widgets/visual/image.h"

namespace vox::editor::ui {
Inspector::Inspector(const std::string &p_title, bool p_opened, const PanelWindowSettings &p_windowSettings)
    : PanelWindow(p_title, p_opened, p_windowSettings) {
    _inspectorHeader = &CreateWidget<Group>();
    _inspectorHeader->enabled_ = false;
    _entityInfo = &CreateWidget<Group>();

    auto &headerColumns = _inspectorHeader->CreateWidget<Columns<2>>();

    /* Name field */
    auto nameGatherer = [this] { return _targetEntity ? _targetEntity->name : "%undef%"; };
    auto nameProvider = [this](const std::string &p_newName) {
        if (_targetEntity) _targetEntity->name = p_newName;
    };
    GuiDrawer::DrawString(headerColumns, "Name", nameGatherer, nameProvider);

    /* Active field */
    auto activeGatherer = [this] { return _targetEntity ? _targetEntity->isActive() : false; };
    auto activeProvider = [this](bool p_active) {
        if (_targetEntity) _targetEntity->setIsActive(p_active);
    };
    GuiDrawer::DrawBoolean(headerColumns, "Active", activeGatherer, activeProvider);
}

Inspector::~Inspector() {
    Entity::destroyedEvent -= _destroyedListener;
    unFocus();
}

void Inspector::focusEntity(Entity *p_target) {
    if (_targetEntity) unFocus();

    _entityInfo->RemoveAllWidgets();

    _targetEntity = p_target;

    _componentAddedListener = _targetEntity->componentAddedEvent +=
            [this](auto useless) { EditorActions::getSingleton().delayAction([this] { refresh(); }); };
    _behaviourAddedListener = _targetEntity->behaviourAddedEvent +=
            [this](auto useless) { EditorActions::getSingleton().delayAction([this] { refresh(); }); };
    _componentRemovedListener = _targetEntity->componentRemovedEvent +=
            [this](auto useless) { EditorActions::getSingleton().delayAction([this] { refresh(); }); };
    _behaviourRemovedListener = _targetEntity->behaviourRemovedEvent +=
            [this](auto useless) { EditorActions::getSingleton().delayAction([this] { refresh(); }); };

    _inspectorHeader->enabled_ = true;

    createEntityInspector(p_target);

    // Force component and script selectors to trigger their ChangedEvent to update button states
    _componentSelectorWidget->value_changed_event_.Invoke(_componentSelectorWidget->current_choice_);
    _scriptSelectorWidget->content_changed_event_.Invoke(_scriptSelectorWidget->content_);

    EditorActions::getSingleton().entitySelectedEvent.Invoke(_targetEntity);
}

void Inspector::unFocus() {
    if (_targetEntity) {
        _targetEntity->componentAddedEvent -= _componentAddedListener;
        _targetEntity->componentRemovedEvent -= _componentRemovedListener;
        _targetEntity->behaviourAddedEvent -= _behaviourAddedListener;
        _targetEntity->behaviourRemovedEvent -= _behaviourRemovedListener;
    }

    softUnFocus();
}

void Inspector::softUnFocus() {
    if (_targetEntity) {
        EditorActions::getSingleton().entityUnselectedEvent.Invoke(_targetEntity);
        _inspectorHeader->enabled_ = false;
        _targetEntity = nullptr;
        _entityInfo->RemoveAllWidgets();
    }
}

Entity *Inspector::targetEntity() const { return _targetEntity; }

void Inspector::createEntityInspector(Entity *&p_target) {
    std::map<std::string, Component *> components;

    //    for (auto &component: p_target->_components)
    //        if (component->name() != "Transform")
    //            components[component->name()] = component.get();

    auto transform = p_target->getComponent<Transform>();
    if (transform) drawComponent(transform);

    for (auto &[name, instance] : components) drawComponent(instance);

    //    auto &behaviours = p_target.GetBehaviours();
    //
    //    for (auto&[name, behaviour]: behaviours)
    //        DrawBehaviour(behaviour);
}

void Inspector::drawComponent(Component *p_component) {
    if (auto inspectorItem = dynamic_cast<InspectorItem *>(p_component); inspectorItem) {
        auto &header = _entityInfo->CreateWidget<GroupCollapsable>(p_component->name());
        header.closable_ = !dynamic_cast<Transform *>(p_component);
        header.close_event_ += [this, &header, &p_component] {
            //            if (p_component->entity()->_removeComponent(p_component))
            //                _componentSelectorWidget->value_changed_event_.invoke(_componentSelectorWidget->currentChoice);
        };
        auto &columns = header.CreateWidget<Columns<2>>();
        columns.widths_[0] = 200;
        inspectorItem->onInspector(columns);
    }
}

void Inspector::drawBehaviour(Behaviour *p_behaviour) {
    if (auto inspectorItem = dynamic_cast<InspectorItem *>(p_behaviour); inspectorItem) {
        auto &header = _entityInfo->CreateWidget<GroupCollapsable>(p_behaviour->name());
        header.closable_ = true;
        header.close_event_ += [this, &header, &p_behaviour] {
            //            p_behaviour->entity()->removeBehaviour(p_behaviour);
        };

        auto &columns = header.CreateWidget<Columns<2>>();
        columns.widths_[0] = 200;
        inspectorItem->onInspector(columns);
    }
}

void Inspector::refresh() {
    if (_targetEntity) {
        _entityInfo->RemoveAllWidgets();
        createEntityInspector(_targetEntity);
    }
}

}  // namespace vox
