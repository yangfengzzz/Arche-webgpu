//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/plugins/plugin.h"
#include "vox.render/ui/widgets/menu/menu_item.h"
#include "vox.render/ui/widgets/menu/menu_list.h"
#include "vox.render/ui/widgets/widget_container.h"

namespace vox::ui {
/**
 * A simple plugin that will show a contextual menu on right click
 * You can add widgets to a contextual menu
 */
class ContextualMenu : public Plugin, public WidgetContainer {
public:
    /**
     * Execute the plugin
     */
    void Execute() override;

    /**
     * Force close the contextual menu
     */
    static void Close();
};

}  // namespace vox::ui
