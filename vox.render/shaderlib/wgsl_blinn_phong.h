//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "functors/wgsl_begin_mobile_frag.h"
#include "functors/wgsl_begin_normal_vert.h"
#include "functors/wgsl_begin_position_vert.h"
#include "functors/wgsl_begin_viewdir_frag.h"
#include "functors/wgsl_blend_shape_input.h"
#include "functors/wgsl_blend_shape_vert.h"
#include "functors/wgsl_color_share.h"
#include "functors/wgsl_color_vert.h"
#include "functors/wgsl_common.h"
#include "functors/wgsl_common_frag.h"
#include "functors/wgsl_common_vert.h"
#include "functors/wgsl_light_frag_define.h"
#include "functors/wgsl_mobile_blinnphong_frag.h"
#include "functors/wgsl_mobile_material_frag.h"
#include "functors/wgsl_normal_get.h"
#include "functors/wgsl_normal_share.h"
#include "functors/wgsl_normal_vert.h"
#include "functors/wgsl_position_vert.h"
#include "functors/wgsl_skinning_vert.h"
#include "functors/wgsl_uv_share.h"
#include "functors/wgsl_uv_vert.h"
#include "functors/wgsl_worldpos_share.h"
#include "functors/wgsl_worldpos_vert.h"
#include "lighting/wgsl/wgsl_cluster_common.h"
#include "lighting/wgsl/wgsl_cluster_compute.h"
#include "shadow/wgsl/wgsl_shadow_common.h"
#include "shadow/wgsl/wgsl_shadow_frag.h"
#include "shadow/wgsl/wgsl_shadow_share.h"
#include "shadow/wgsl/wgsl_shadow_vert.h"
#include "wgsl_cache.h"

namespace vox {
// MARK: - Unlit Vertex Code
class WGSLBlinnPhongVertex : public WGSLCache {
public:
    WGSLBlinnPhongVertex();

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

    WGSLShadowShare _shadowShare;
    WGSLShadowVert _shadowVert;
};

// MARK: - Unlit Fragment Code
class WGSLBlinnPhongFragment : public WGSLCache {
public:
    WGSLBlinnPhongFragment();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommon _common;
    WGSLCommonFrag _commonFrag;
    WGSLUVShare _uvShare;
    WGSLColorShare _colorShare;
    WGSLNormalShare _normalShare;
    WGSLWorldPosShare _worldPosShare;
    WGSLLightFragDefine _lightFragDefine;
    WGSLMobileMaterialShare _mobileMaterialShare;
    WGSLNormalGet _normalGet;

    WGSLBeginMobileFrag _beginMobileFrag;
    WGSLBeginViewDirFrag _beginViewDirFrag;
    WGSLMobileBlinnphongFrag _mobileBlinnphoneFrag;

    WGSLShadowShare _shadowShare;
    WGSLShadowCommon _shadowCommon;
    WGSLShadowFrag _shadowFrag;

    WGSLForwardPlusUniforms _forwardPlusUniforms;
    WGSLTileFunctions _tileFunctions;
    WGSLClusterLightsStructs _clusterLightsStructs;
};

}  // namespace vox