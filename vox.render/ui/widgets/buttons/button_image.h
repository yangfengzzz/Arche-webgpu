//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>
#include "vox.math/color.h"
#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/buttons/button.h"

namespace vox::ui {
/**
 * Button widget with an image
 */
class ButtonImage : public Button {
public:
    ButtonImage(wgpu::TextureView p_textureID, const Vector2F &p_size);
    
protected:
    void DrawImpl() override;

public:
    bool disabled_ = false;

    Color background_ = {0, 0, 0, 0};
    Color tint_ = {1, 1, 1, 1};

    wgpu::TextureView textureID;
    Vector2F size_;
};

}  // namespace vox::ui
