//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef inspector_item_hpp
#define inspector_item_hpp

#include "vobject.h"
#include "ui/widgets/widget_container.h"

namespace vox {
/**
 * IInspectorItem is an interface to any class that is serializable and drawable on inspector
 */
class InspectorItem : public VObject {
public:
    /**
     * Called when the inspector needs to create the UI elements for the given IInspectorItem
     * @param p_root p_root
     */
    virtual void onInspector(ui::WidgetContainer &p_root) = 0;
};


}
#endif /* inspector_item_hpp */
