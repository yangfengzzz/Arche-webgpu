//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef unlit_material_hpp
#define unlit_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"

namespace vox {
/**
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    Color baseColor();
    
    void setBaseColor(const Color &newValue);
    
    /**
     * Base texture.
     */
    std::shared_ptr<MTL::Texture> baseTexture();
    
    void setBaseTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    Vector4F tilingOffset();
    
    void setTilingOffset(const Vector4F &newValue);
    
    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial();
    
private:
    ShaderProperty _baseColorProp;
    ShaderProperty _baseTextureProp;
    ShaderProperty _tilingOffsetProp;
};

}

#endif /* unlit_material_hpp */
