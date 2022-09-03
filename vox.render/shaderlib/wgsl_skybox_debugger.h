//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "functors/wgsl_begin_position_vert.h"
#include "functors/wgsl_common.h"
#include "functors/wgsl_common_vert.h"
#include "functors/wgsl_position_vert.h"
#include "functors/wgsl_uv_share.h"
#include "functors/wgsl_uv_vert.h"
#include "wgsl_cache.h"

namespace vox {
// MARK: - Unlit Vertex Code
class WGSLSkyboxDebuggerVertex : public WGSLCache {
public:
    WGSLSkyboxDebuggerVertex();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommonVert _commonVert;
    WGSLUVShare _uvShare;
    WGSLBeginPositionVert _beginPositionVert;
    WGSLUVVert _uvVert;
    WGSLPositionVert _positionVert;
};

// MARK: - Unlit Fragment Code
class WGSLSkyboxDebuggerFragment : public WGSLCache {
public:
    WGSLSkyboxDebuggerFragment();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLCommon _common;
    WGSLUVShare _uvShare;
};

}  // namespace vox