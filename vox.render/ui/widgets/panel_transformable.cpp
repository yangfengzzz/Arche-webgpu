//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_transformable.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
PanelTransformable::PanelTransformable(const Vector2F &p_defaultPosition,
                                       const Vector2F &p_defaultSize,
                                       HorizontalAlignment p_defaultHorizontalAlignment,
                                       VerticalAlignment p_defaultVerticalAlignment,
                                       bool p_ignoreConfigFile) :
_defaultPosition(p_defaultPosition),
_defaultSize(p_defaultSize),
_defaultHorizontalAlignment(p_defaultHorizontalAlignment),
_defaultVerticalAlignment(p_defaultVerticalAlignment),
_ignoreConfigFile(p_ignoreConfigFile) {
}

void PanelTransformable::setPosition(const Vector2F &p_position) {
    _position = p_position;
    _positionChanged = true;
}

void PanelTransformable::setSize(const Vector2F &p_size) {
    _size = p_size;
    _sizeChanged = true;
}

void PanelTransformable::setAlignment(HorizontalAlignment p_horizontalAlignment, VerticalAlignment p_verticalAligment) {
    _horizontalAlignment = p_horizontalAlignment;
    _verticalAlignment = p_verticalAligment;
    _alignmentChanged = true;
}

const Vector2F &PanelTransformable::position() const {
    return _position;
}

const Vector2F &PanelTransformable::size() const {
    return _size;
}

HorizontalAlignment PanelTransformable::horizontalAlignment() const {
    return _horizontalAlignment;
}

VerticalAlignment PanelTransformable::verticalAlignment() const {
    return _verticalAlignment;
}

void PanelTransformable::updatePosition() {
    if (_defaultPosition.x != -1.f && _defaultPosition.y != 1.f) {
        Vector2F offsettedDefaultPos = _defaultPosition + calculatePositionAlignmentOffset(true);
        ImGui::SetWindowPos(Converter::ToImVec2(offsettedDefaultPos), _ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
    }
    
    if (_positionChanged || _alignmentChanged) {
        Vector2F offset = calculatePositionAlignmentOffset(false);
        Vector2F offsettedPos(_position.x + offset.x, _position.y + offset.y);
        ImGui::SetWindowPos(Converter::ToImVec2(offsettedPos), ImGuiCond_Always);
        _positionChanged = false;
        _alignmentChanged = false;
    }
}

void PanelTransformable::updateSize() {
    /*
     if (_defaultSize.x != -1.f && _defaultSize.y != 1.f)
     ImGui::SetWindowSize(Internal::Converter::ToImVec2(_defaultSize), _ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
     */
    if (_sizeChanged) {
        ImGui::SetWindowSize(Converter::ToImVec2(_size), ImGuiCond_Always);
        _sizeChanged = false;
    }
}

void PanelTransformable::copyImGuiPosition() {
    _position = Converter::ToVector2F(ImGui::GetWindowPos());
}

void PanelTransformable::copyImGuiSize() {
    _size = Converter::ToVector2F(ImGui::GetWindowSize());
}

void PanelTransformable::update() {
    if (!_firstFrame) {
        if (!autoSize)
            updateSize();
        copyImGuiSize();
        
        updatePosition();
        copyImGuiPosition();
    }
    
    _firstFrame = false;
}

Vector2F PanelTransformable::calculatePositionAlignmentOffset(bool p_default) {
    Vector2F result(0.0f, 0.0f);
    
    switch (p_default ? _defaultHorizontalAlignment : _horizontalAlignment) {
        case HorizontalAlignment::CENTER:
            result.x -= _size.x / 2.0f;
            break;
        case HorizontalAlignment::RIGHT:
            result.x -= _size.x;
            break;
        default:
            break;
    }
    
    switch (p_default ? _defaultVerticalAlignment : _verticalAlignment) {
        case VerticalAlignment::MIDDLE:
            result.y -= _size.y / 2.0f;
            break;
        case VerticalAlignment::BOTTOM:
            result.y -= _size.y;
            break;
        default:
            break;
    }
    
    return result;
}


}
}
