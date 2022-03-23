//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "inspector.h"
#include "entity.h"

#include "ui/widgets/texts/text.h"
#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/drags/drag_multiple_floats.h"
#include "ui/widgets/drags/drag_float.h"
#include "ui/widgets/layout/spacing.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/visual/image.h"
#include "ui/widgets/input_fields/input_float.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/plugins/drag_drop_target.h"
#include "ui/gui_drawer.h"

namespace vox {
namespace editor {
namespace ui {
Inspector::Inspector(const std::string& p_title,
                     bool p_opened,
                     const PanelWindowSettings & p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings) {
    _inspectorHeader = &createWidget<Group>();
    _inspectorHeader->enabled = false;
    _actorInfo = &createWidget<Group>();
    
    auto& headerColumns = _inspectorHeader->createWidget<Columns<2>>();
    
    /* Name field */
    auto nameGatherer = [this] { return _targetActor ? _targetActor->name : "%undef%"; };
    auto nameProvider = [this](const std::string& p_newName) { if (_targetActor) _targetActor->name = p_newName; };
    GUIDrawer::drawString(headerColumns, "Name", nameGatherer, nameProvider);
    
    /* Active field */
    auto activeGatherer = [this] { return _targetActor ? _targetActor->isActive() : false; };
    auto activeProvider = [this](bool p_active) { if (_targetActor) _targetActor->setIsActive(p_active); };
    GUIDrawer::drawBoolean(headerColumns, "Active", activeGatherer, activeProvider);
    
}

Inspector::~Inspector() {
    // Entity::DestroyedEvent -= _destroyedListener;
    unFocus();
}



}
}
}
