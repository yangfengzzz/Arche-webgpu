//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_shadow_debug_hpp
#define wgsl_shadow_debug_hpp

#include "shaderlib/functors/wgsl_color_share.h"
#include "shaderlib/functors/wgsl_common_frag.h"
#include "shaderlib/functors/wgsl_normal_share.h"
#include "shaderlib/functors/wgsl_uv_share.h"
#include "shaderlib/functors/wgsl_worldpos_share.h"
#include "shadow/wgsl/wgsl_shadow_common.h"
#include "shadow/wgsl/wgsl_shadow_frag.h"
#include "shadow/wgsl/wgsl_shadow_share.h"
#include "vox.render/shaderlib/wgsl_cache.h"

namespace vox {
class WGSLShadowDebug : public WGSLCache {
public:
    WGSLShadowDebug();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommonFrag _commonFrag;
    WGSLUVShare _uvShare;
    WGSLColorShare _colorShare;
    WGSLNormalShare _normalShare;
    WGSLWorldPosShare _worldPosShare;

    WGSLShadowCommon _shadowCommon;
    WGSLShadowShare _shadowShare;
    WGSLShadowFrag _shadowFrag;
};

}  // namespace vox
#endif /* wgsl_shadow_debug_hpp */
