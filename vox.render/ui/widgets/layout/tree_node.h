//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef tree_node_hpp
#define tree_node_hpp

#include "ui/widgets/widget_container.h"
#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget that allow columnification
 */
class TreeNode : public DataWidget<std::string>, public WidgetContainer {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_arrowClickToOpen p_arrowClickToOpen
     */
    TreeNode(const std::string &p_name = "", bool p_arrowClickToOpen = false);
    
    /**
     * Open the tree node
     */
    void open();
    
    /**
     * Close the tree node
     */
    void close();
    
    /**
     * Returns true if the TreeNode is currently opened
     */
    bool isOpened() const;
    
protected:
    void _draw_Impl() override;
    
public:
    std::string name;
    bool selected = false;
    bool leaf = false;
    
    Event<> clickedEvent;
    Event<> doubleClickedEvent;
    Event<> openedEvent;
    Event<> closedEvent;
    
private:
    bool _arrowClickToOpen = false;
    bool _shouldOpen = false;
    bool _shouldClose = false;
    bool _opened = false;
};


}
}
#endif /* tree_node_hpp */
