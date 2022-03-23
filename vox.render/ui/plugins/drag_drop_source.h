//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_drop_source_h
#define drag_drop_source_h

namespace vox {
namespace ui {
/**
 * Represents a drag and drop source
 */
template<typename T>
class DDSource : public Plugin {
public:
    /**
     * Create the drag and drop source
     */
    DDSource(const std::string &p_identifier,
             const std::string &p_tooltip,
             T p_data) :
    identifier(p_identifier), tooltip(p_tooltip), data(p_data) {
    }
    
    /**
     * Execute the behaviour of the drag and drop source
     */
    virtual void execute() override {
        ImGuiDragDropFlags src_flags = 0;
        // Keep the source displayed as hovered
        src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
        // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
        src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
        
        if (!hasTooltip)
            src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
        
        if (ImGui::BeginDragDropSource(src_flags)) {
            if (!_isDragged)
                dragStartEvent.invoke();
            
            _isDragged = true;
            
            if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                ImGui::Text(tooltip.c_str());
            ImGui::SetDragDropPayload(identifier.c_str(), &data, sizeof(data));
            ImGui::EndDragDropSource();
        } else {
            if (_isDragged)
                dragStopEvent.invoke();
            
            _isDragged = false;
        }
    }
    
    /**
     * Returns true if the drag and drop source is dragged
     */
    bool isDragged() const {
        return _isDragged;
    }
    
public:
    std::string identifier;
    std::string tooltip;
    T data;
    Event<> dragStartEvent;
    Event<> dragStopEvent;
    
    bool hasTooltip = true;
    
private:
    bool _isDragged;
};


}
}
#endif /* drag_drop_source_h */
