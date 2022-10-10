//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Simple widget that display an image
 */
class ImageUI : public Widget {
public:
    /**
     * Constructor
     * @param p_textureID p_textureID
     * @parma p_size p_size
     */
    ImageUI(wgpu::TextureView p_textureID, const Vector2F &p_size);

    void SetTextureView(wgpu::TextureView p_textureID);

protected:
    void DrawImpl() override;

public:
    wgpu::TextureView textureID;
    Vector2F size;
};

}  // namespace vox::ui
