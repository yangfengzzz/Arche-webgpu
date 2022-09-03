//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/lighting/wgsl/wgsl_cluster_compute.h"
#include "vox.render/shaderlib/functors/wgsl_uv_share.h"
#include "vox.render/shaderlib/wgsl_cache.h"

namespace vox {
class WGSLClusterDebug : public WGSLCache {
public:
    WGSLClusterDebug(const std::array<uint32_t, 3>& tileCount, uint32_t maxLightsPerCluster);

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLUVShare _uvShare;
    WGSLForwardPlusUniforms _forwardPlusUniforms;
    WGSLTileFunctions _tileFunctions;
    WGSLClusterLightsStructs _clusterLightsStructs;

    uint32_t _maxLightsPerCluster;
};

}  // namespace vox