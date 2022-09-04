//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector4.h"
#include "vox.render/material/enums/blend_mode.h"
#include "vox.render/material/enums/render_face.h"
#include "vox.render/material/material.h"

namespace vox {
class BaseMaterial : public Material {
public:
    /**
     * Is this material transparent.
     * @remarks
     * If material is transparent, transparent blend mode will be affected by `blendMode`, default is
     * `BlendMode.Normal`.
     */
    [[nodiscard]] bool isTransparent() const;

    void setIsTransparent(bool newValue);

    /**
     * Alpha cutoff value.
     * @remarks
     * Fragments with alpha channel lower than cutoff value will be discarded.
     * `0` means no fragment will be discarded.
     */
    [[nodiscard]] float alphaCutoff() const;

    void setAlphaCutoff(float newValue);

    /**
     * Tiling and offset of main textures.
     */
    const Vector4F &tilingOffset();

    void setTilingOffset(const Vector4F &newValue);

    /**
     * Set which face for render.
     */
    const RenderFace &renderFace();

    void setRenderFace(const RenderFace &newValue);

    /**
     * Alpha blend mode.
     * @remarks
     * Only take effect when `isTransparent` is `true`.
     */
    const BlendMode &blendMode();

    void setBlendMode(const BlendMode &newValue);

    /**
     * Create a BaseMaterial instance.
     */
    explicit BaseMaterial(wgpu::Device &device, const std::string &name = "");

private:
    float _alphaCutoff = 0.0;
    const std::string _alphaCutoffProp;

    Vector4F _tilingOffset = Vector4F(1, 1, 0, 0);
    const std::string _tilingOffsetProp;

    RenderFace _renderFace = RenderFace::Back;
    BlendMode _blendMode = BlendMode::Normal;
    bool _isTransparent = false;
};

}  // namespace vox