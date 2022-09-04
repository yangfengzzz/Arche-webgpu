//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/widget_container.h"
#include "vox.render/vobject.h"

namespace vox {
/**
 * IInspectorItem is an interface to any class that is serializable and drawable on inspector
 */
class InspectorItem : public VObject {
public:
    /**
     * Returns the name of the component
     */
    virtual std::string name() = 0;

    /**
     * Called when the inspector needs to create the UI elements for the given IInspectorItem
     * @param p_root p_root
     */
    virtual void onInspector(ui::WidgetContainer &p_root) = 0;
};

}  // namespace vox
