//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_drop_target_h
#define drag_drop_target_h

#include "event.h"
#include "plugin.h"

namespace vox {
namespace ui {
/**
 * Represents a drag and drop target
 */
template<typename T>
class DDTarget : public Plugin {
public:
    /**
     * Create the drag and drop target
     * @param p_identifier p_identifier
     */
    DDTarget(const std::string &p_identifier) : identifier(p_identifier) {
    }
    
    /**
     * Execute the drag and drop target behaviour
     */
    virtual void execute() override {
        if (ImGui::BeginDragDropTarget()) {
            if (!_isHovered)
                hoverStartEvent.invoke();
            
            _isHovered = true;
            
            ImGuiDragDropFlags target_flags = 0;
            // Don't wait until the delivery (release mouse button on a target) to do something
            // target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
            
            if (!showYellowRect)
                target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
            
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(identifier.c_str(), target_flags)) {
                T data = *(T *) payload->Data;
                dataReceivedEvent.invoke(data);
            }
            ImGui::EndDragDropTarget();
        } else {
            if (_isHovered)
                hoverEndEvent.invoke();
            
            _isHovered = false;
        }
    }
    
    /**
     * Returns true if the drag and drop target is hovered by a drag and drop source
     */
    bool IsHovered() const {
        return _isHovered;
    }
    
public:
    std::string identifier;
    Event<T> dataReceivedEvent;
    Event<> hoverStartEvent;
    Event<> hoverEndEvent;
    
    bool showYellowRect = true;
    
private:
    bool _isHovered;
};


}
}
#endif /* drag_drop_target_h */
