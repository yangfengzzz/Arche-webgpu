//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_shadow_hpp
#define wgsl_shadow_hpp

#include "shaderlib/wgsl_cache.h"
#include "shaderlib/functors/wgsl_common_vert.h"
#include "shaderlib/functors/wgsl_blend_shape_input.h"
#include "shaderlib/functors/wgsl_begin_position_vert.h"
#include "shaderlib/functors/wgsl_blend_shape_vert.h"
#include "shaderlib/functors/wgsl_skinning_vert.h"

namespace vox {
//MARK: - Shadow Vertex Code
class WGSLShadowVertex : public WGSLCache {
public:
    WGSLShadowVertex();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommonVert _commonVert;
    WGSLBlendShapeInput _blendShapeInput;
    WGSLBeginPositionVert _beginPositionVert;
    WGSLBlendShapeVert _blendShapeVert;
    WGSLSkinningVert _skinningVert;
};
}

#endif /* wgsl_shadow_hpp */
