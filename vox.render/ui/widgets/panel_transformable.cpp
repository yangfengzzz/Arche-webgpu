//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/panel_transformable.h"

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
PanelTransformable::PanelTransformable(const Vector2F &default_position,
                                       const Vector2F &default_size,
                                       HorizontalAlignment default_horizontal_alignment,
                                       VerticalAlignment default_vertical_alignment,
                                       bool ignore_config_file)
    : default_position_(default_position),
      default_size_(default_size),
      default_horizontal_alignment_(default_horizontal_alignment),
      default_vertical_alignment_(default_vertical_alignment),
      ignore_config_file_(ignore_config_file) {}

void PanelTransformable::SetPosition(const Vector2F &position) {
    position_ = position;
    position_changed_ = true;
}

void PanelTransformable::SetSize(const Vector2F &size) {
    size_ = size;
    size_changed_ = true;
}

void PanelTransformable::SetAlignment(HorizontalAlignment horizontal_alignment, VerticalAlignment vertical_alignment) {
    horizontal_alignment_ = horizontal_alignment;
    vertical_alignment_ = vertical_alignment;
    alignment_changed_ = true;
}

const Vector2F &PanelTransformable::Position() const { return position_; }

const Vector2F &PanelTransformable::Size() const { return size_; }

HorizontalAlignment PanelTransformable::GetHorizontalAlignment() const { return horizontal_alignment_; }

VerticalAlignment PanelTransformable::GetVerticalAlignment() const { return vertical_alignment_; }

void PanelTransformable::UpdatePosition() {
    if (default_position_.x != -1.f && default_position_.y != 1.f) {
        Vector2F offsetted_default_pos = default_position_ + CalculatePositionAlignmentOffset(true);
        ImGui::SetWindowPos(Converter::ToImVec2(offsetted_default_pos),
                            ignore_config_file_ ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
    }

    if (position_changed_ || alignment_changed_) {
        Vector2F offset = CalculatePositionAlignmentOffset(false);
        Vector2F offsetted_pos(position_.x + offset.x, position_.y + offset.y);
        ImGui::SetWindowPos(Converter::ToImVec2(offsetted_pos), ImGuiCond_Always);
        position_changed_ = false;
        alignment_changed_ = false;
    }
}

void PanelTransformable::UpdateSize() {
    /*
     if (_defaultSize.x != -1.f && _defaultSize.y != 1.f)
     ImGui::SetWindowSize(Internal::Converter::to_imVec2(_defaultSize), _ignoreConfigFile ? ImGuiCond_Once :
     ImGuiCond_FirstUseEver);
     */
    if (size_changed_) {
        ImGui::SetWindowSize(Converter::ToImVec2(size_), ImGuiCond_Always);
        size_changed_ = false;
    }
}

void PanelTransformable::CopyImGuiPosition() { position_ = Converter::ToVector2F(ImGui::GetWindowPos()); }

void PanelTransformable::CopyImGuiSize() { size_ = Converter::ToVector2F(ImGui::GetWindowSize()); }

void PanelTransformable::Update() {
    if (!first_frame_) {
        if (!auto_size_) UpdateSize();
        CopyImGuiSize();

        UpdatePosition();
        CopyImGuiPosition();
    }

    first_frame_ = false;
}

Vector2F PanelTransformable::CalculatePositionAlignmentOffset(bool is_default) {
    Vector2F result(0.0f, 0.0f);

    switch (is_default ? default_horizontal_alignment_ : horizontal_alignment_) {
        case HorizontalAlignment::CENTER:
            result.x -= size_.x / 2.0f;
            break;
        case HorizontalAlignment::RIGHT:
            result.x -= size_.x;
            break;
        default:
            break;
    }

    switch (is_default ? default_vertical_alignment_ : vertical_alignment_) {
        case VerticalAlignment::MIDDLE:
            result.y -= size_.y / 2.0f;
            break;
        case VerticalAlignment::BOTTOM:
            result.y -= size_.y;
            break;
        default:
            break;
    }

    return result;
}

}  // namespace vox::ui
