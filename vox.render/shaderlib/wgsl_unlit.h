//
//  wgsl_unlit.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_unlit_hpp
#define wgsl_unlit_hpp

#include "wgsl_cache.h"
#include "functors/wgsl_common.h"
#include "functors/wgsl_common_vert.h"
#include "functors/wgsl_blend_shape_input.h"
#include "functors/wgsl_uv_share.h"
#include "functors/wgsl_begin_position_vert.h"
#include "functors/wgsl_blend_shape_vert.h"
#include "functors/wgsl_skinning_vert.h"
#include "functors/wgsl_uv_vert.h"
#include "functors/wgsl_position_vert.h"

namespace vox {
//MARK: - Unlit Vertex Code
class WGSLUnlitVertex : public WGSLCache {
public:
    WGSLUnlitVertex();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommonVert _commonVert;
    WGSLBlendShapeInput _blendShapeInput;
    WGSLUVShare _uvShare;
    WGSLBeginPositionVert _beginPositionVert;
    WGSLBlendShapeVert _blendShapeVert;
    WGSLSkinningVert _skinningVert;
    WGSLUVVert _uvVert;
    WGSLPositionVert _positionVert;
};

//MARK: - Unlit Fragment Code
class WGSLUnlitFragment : public WGSLCache {
public:
    WGSLUnlitFragment();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommon _common;
    WGSLUVShare _uvShare;
};

}

#endif /* wgsl_unlit_hpp */
