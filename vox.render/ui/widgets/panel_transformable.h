//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_transformable_hpp
#define panel_transformable_hpp

#include <vector>
#include <memory>

#include "vector2.h"
#include "event.h"

#include "ui/widgets/panel.h"
#include "ui/widgets/alignment.h"

namespace vox {
namespace ui {
/**
 * APanelTransformable is a panel that is localized in the canvas
 */
class PanelTransformable : public Panel {
public:
    /**
     * Create a APanelTransformable
     * @param p_defaultPosition p_defaultPosition
     * @param p_defaultSize p_defaultSize
     * @param p_defaultHorizontalAlignment p_defaultHorizontalAlignment
     * @param p_defaultVerticalAlignment p_defaultVerticalAlignment
     * @param p_ignoreConfigFile p_ignoreConfigFile
     */
    PanelTransformable(const Vector2F &p_defaultPosition = Vector2F(-1.f, -1.f),
                       const Vector2F &p_defaultSize = Vector2F(-1.f, -1.f),
                       HorizontalAlignment p_defaultHorizontalAlignment = HorizontalAlignment::LEFT,
                       VerticalAlignment p_defaultVerticalAlignment = VerticalAlignment::TOP,
                       bool p_ignoreConfigFile = false);
    
    /**
     * Defines the position of the panel
     * @param p_position p_position
     */
    void setPosition(const Vector2F &p_position);
    
    /**
     * Defines the size of the panel
     * @param p_size p_size
     */
    void setSize(const Vector2F &p_size);
    
    /**
     * Defines the alignment of the panel
     * @param p_horizontalAlignment p_horizontalAlignment
     * @param p_verticalAligment p_verticalAligment
     */
    void setAlignment(HorizontalAlignment p_horizontalAlignment,
                      VerticalAlignment p_verticalAligment);
    
    /**
     * Returns the current position of the panel
     */
    const Vector2F &position() const;
    
    /**
     * Returns the current size of the panel
     */
    const Vector2F &size() const;
    
    /**
     * Returns the current horizontal alignment of the panel
     */
    HorizontalAlignment horizontalAlignment() const;
    
    /**
     * Returns the current vertical alignment of the panel
     */
    VerticalAlignment verticalAlignment() const;
    
protected:
    void update();
    
    virtual void _draw_Impl() = 0;
    
private:
    Vector2F calculatePositionAlignmentOffset(bool p_default = false);
    
    void updatePosition();
    
    void updateSize();
    
    void copyImGuiPosition();
    
    void copyImGuiSize();
    
public:
    bool autoSize = true;
    
protected:
    Vector2F _defaultPosition;
    Vector2F _defaultSize;
    HorizontalAlignment _defaultHorizontalAlignment;
    VerticalAlignment _defaultVerticalAlignment;
    bool _ignoreConfigFile;
    
    Vector2F _position = Vector2F(0.0f, 0.0f);
    Vector2F _size = Vector2F(0.0f, 0.0f);
    
    bool _positionChanged = false;
    bool _sizeChanged = false;
    
    HorizontalAlignment _horizontalAlignment = HorizontalAlignment::LEFT;
    VerticalAlignment _verticalAlignment = VerticalAlignment::TOP;
    
    bool _alignmentChanged = false;
    bool _firstFrame = true;
};


}
}
#endif /* panel_transformable_hpp */
