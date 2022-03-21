//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef group_collapsable_hpp
#define group_collapsable_hpp

#include "group.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget that can contains other widgets and is collapsable
 */
class GroupCollapsable : public Group {
public:
    /**
     * Constructor
     */
    GroupCollapsable(const std::string &p_name = "");
    
protected:
    void _draw_Impl() override;
    
public:
    std::string name;
    bool closable = false;
    bool opened = true;
    Event<> closeEvent;
    Event<> openEvent;
};

}
}
#endif /* group_collapsable_hpp */
