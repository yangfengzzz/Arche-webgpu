//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef base_material_hpp
#define base_material_hpp

#include "material.h"
#include "enums/render_face.h"
#include "enums/blend_mode.h"
#include "vector4.h"

namespace vox {
class BaseMaterial : public Material {
public:
    /**
     * Is this material transparent.
     * @remarks
     * If material is transparent, transparent blend mode will be affected by `blendMode`, default is `BlendMode.Normal`.
     */
    bool isTransparent();
    
    void setIsTransparent(bool newValue);
    
    /**
     * Alpha cutoff value.
     * @remarks
     * Fragments with alpha channel lower than cutoff value will be discarded.
     * `0` means no fragment will be discarded.
     */
    float alphaCutoff();
    
    void setAlphaCutoff(float newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    const Vector4F& tilingOffset();
    
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
     * @param shader - Shader used by the material
     */
    BaseMaterial(wgpu::Device& device, Shader *shader);
    
private:
    float _alphaCutoff = 0.0;
    ShaderProperty _alphaCutoffProp;
    
    Vector4F _tilingOffset = Vector4F(1, 1, 0, 0);
    ShaderProperty _tilingOffsetProp;
    
    RenderFace _renderFace = RenderFace::Back;
    BlendMode _blendMode = BlendMode::Normal;
    bool _isTransparent = false;
};

}
#endif /* base_material_hpp */
