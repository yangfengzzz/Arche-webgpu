//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sprite_debug_material_hpp
#define sprite_debug_material_hpp

#include "material/base_material.h"
#include "shaderlib/functors/wgsl_light_frag_define.h"
#include "shaderlib/functors/wgsl_common_frag.h"
#include "vox.render/shaderlib/wgsl_cache.h"

namespace vox {
//MARK: - Unlit Vertex Code
class WGSLSpriteDebugVertex : public WGSLCache {
public:
    WGSLSpriteDebugVertex(bool isSpotLight);
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    bool _isSpotLight;
    WGSLLightFragDefine _lightFragDefine;
    WGSLCommonFrag _commonVert;
};

//MARK: - Unlit Fragment Code
class WGSLSpriteDebugFragment : public WGSLCache {        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
};

//MARK: - SpriteDebugMaterial
class SpriteDebugMaterial : public BaseMaterial {
public:
    SpriteDebugMaterial(wgpu::Device& device, bool isSpotLight);
};

}
#endif /* sprite_debug_material_hpp */
