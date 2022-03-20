//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef columns_hpp
#define columns_hpp

#include <array>

#include "ui/widgets/widget_container.h"

namespace vox {
namespace ui {
/**
 * Widget that allow columnification
 */
template<size_t _Size>
class Columns : public Widget, public WidgetContainer {
public:
    /**
     * Constructor
     */
    Columns() {
        widths.fill(-1.f);
    }
    
protected:
    virtual void _draw_Impl() override {
        ImGui::Columns(static_cast<int>(_Size), ("##" + _widgetID).c_str(), false);
        
        int counter = 0;
        
        collectGarbages();
        
        for (auto it = _widgets.begin(); it != _widgets.end();) {
            it->first->draw();
            
            ++it;
            
            if (it != _widgets.end()) {
                if (widths[counter] != -1.f)
                    ImGui::SetColumnWidth(counter, widths[counter]);
                
                ImGui::NextColumn();
            }
            
            ++counter;
            
            if (counter == _Size)
                counter = 0;
        }
        
        ImGui::Columns(1); // Necessary to not break the layout for following widget
    }
    
public:
    std::array<float, _Size> widths;
};

}
}
#endif /* columns_hpp */
