//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/wgsl/wgsl_shadow_debug.h"

namespace vox {
WGSLShadowDebug::WGSLShadowDebug()
    : _commonFrag("VertexOut"),
      _uvShare("VertexOut"),
      _colorShare("VertexOut"),
      _normalShare("VertexOut"),
      _worldPosShare("VertexOut"),

      _shadowShare("VertexOut"),
      _shadowFrag() {}

void WGSLShadowDebug::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        _shadowCommon(encoder, macros);

        _commonFrag(encoder, macros);
        _shadowFrag(encoder, macros);

        _uvShare(encoder, macros, inputStructCounter);
        _colorShare(encoder, macros, inputStructCounter);
        _normalShare(encoder, macros, inputStructCounter);
        _worldPosShare(encoder, macros, inputStructCounter);
        _shadowShare(encoder, macros, inputStructCounter);

        encoder.addFunction(
                "fn LinearizeDepth(depth: f32)->f32 {\n"
                "  let n = 1.0;\n"    // camera z near
                "  let f = 128.0;\n"  // camera z far
                "  let z = depth;\n"
                "  return (2.0 * n) / (f + n - z * (f - n));\n"
                "}\n");

        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"}, [&](std::string& source) {
            // Get cascade index for the current fragment's view position
            source +=
                    "var cascadeIndex = 0u;\n"
                    "for(var i = 0u; i < 4u - 1u; i = i + 1u) {\n"
                    "  if(in.view_pos.z < u_shadowData[0].cascadeSplits[i]) {\n"
                    "    cascadeIndex = i + 1u;\n"
                    "  }\n"
                    "}\n"
                    "\n"
                    "if (cascadeIndex == 0u) {\n"
                    "  out.finalColor = vec4<f32>(1.0, 1.0, 1.0, 1.0);\n"
                    "} else if (cascadeIndex == 1u) {\n"
                    "  out.finalColor = vec4<f32>(1.0, 0.0, 0.0, 1.0);\n"
                    "} else if (cascadeIndex == 2u) {\n"
                    "  out.finalColor = vec4<f32>(0.0, 1.0, 0.0, 1.0);\n"
                    "} else if (cascadeIndex == 3u) {\n"
                    "  out.finalColor = vec4<f32>(0.0, 0.0, 1.0, 1.0);\n"
                    "}\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}  // namespace vox
