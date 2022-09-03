//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_cluster_debug.h"
#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLClusterDebug::WGSLClusterDebug(const std::array<uint32_t, 3>& tileCount, uint32_t maxLightsPerCluster):
_forwardPlusUniforms(),
_tileFunctions(tileCount),
_clusterLightsStructs(tileCount[0] * tileCount[1] * tileCount[2], maxLightsPerCluster),
_uvShare("VertexOut"),
_maxLightsPerCluster(maxLightsPerCluster) {
}

void WGSLClusterDebug::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        _forwardPlusUniforms(encoder, macros);
        _tileFunctions(encoder, macros);
        _clusterLightsStructs(encoder, macros);
        
        _uvShare(encoder, macros, inputStructCounter);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "fragCoord", UniformType::Vec4f32);
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "let clusterIndex : u32 = getClusterIndex(in.fragCoord);\n";
            source += "let lightCount : u32 = u_clusterLights.lights[clusterIndex].point_count + u_clusterLights.lights[clusterIndex].spot_count;\n";
            source += fmt::format("let lightFactor : f32 = f32(lightCount) / f32({});\n", _maxLightsPerCluster);
            source += "out.finalColor = mix(vec4<f32>(0.0, 0.0, 1.0, 1.0), vec4<f32>(1.0, 0.0, 0.0, 1.0), vec4<f32>(lightFactor, lightFactor, lightFactor, lightFactor));\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
