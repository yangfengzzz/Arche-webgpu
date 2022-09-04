//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "functors/wgsl_begin_normal_vert.h"
#include "functors/wgsl_begin_position_vert.h"
#include "functors/wgsl_blend_shape_input.h"
#include "functors/wgsl_blend_shape_vert.h"
#include "functors/wgsl_color_share.h"
#include "functors/wgsl_color_vert.h"
#include "functors/wgsl_common.h"
#include "functors/wgsl_common_frag.h"
#include "functors/wgsl_common_vert.h"
#include "functors/wgsl_light_frag_define.h"
#include "functors/wgsl_normal_share.h"
#include "functors/wgsl_normal_vert.h"
#include "functors/wgsl_pbr_frag.h"
#include "functors/wgsl_pbr_frag_define.h"
#include "functors/wgsl_pbr_helper.h"
#include "functors/wgsl_position_vert.h"
#include "functors/wgsl_skinning_vert.h"
#include "functors/wgsl_uv_share.h"
#include "functors/wgsl_uv_vert.h"
#include "functors/wgsl_worldpos_share.h"
#include "functors/wgsl_worldpos_vert.h"
#include "wgsl_cache.h"

namespace vox {
// MARK: - Unlit Vertex Code
class WGSLPbrVertex : public WGSLCache {
public:
    WGSLPbrVertex();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommon _common;
    WGSLCommonVert _commonVert;
    WGSLBlendShapeInput _blendShapeInput;
    WGSLUVShare _uvShare;
    WGSLColorShare _colorShare;
    WGSLNormalShare _normalShare;
    WGSLWorldPosShare _worldPosShare;

    WGSLBeginPositionVert _beginPositionVert;
    WGSLBeginNormalVert _beginNormalVert;
    WGSLBlendShapeVert _blendShapeVert;
    WGSLSkinningVert _skinningVert;
    WGSLUVVert _uvVert;
    WGSLColorVert _colorVert;
    WGSLNormalVert _normalVert;
    WGSLWorldPosVert _worldPosVert;
    WGSLPositionVert _positionVert;
};

// MARK: - Unlit Fragment Code
class WGSLPbrFragment : public WGSLCache {
public:
    WGSLPbrFragment(bool is_metallic_workflow);

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommon _common;
    WGSLCommonFrag _commonFrag;
    WGSLUVShare _uvShare;
    WGSLColorShare _colorShare;
    WGSLNormalShare _normalShare;
    WGSLWorldPosShare _worldPosShare;
    WGSLLightFragDefine _lightFragDefine;
    WGSLPbrFragDefine _pbrFragDefine;
    WGSLPbrHelper _pbrHelper;
    WGSLPbrFrag _pbrFrag;
};

}  // namespace vox