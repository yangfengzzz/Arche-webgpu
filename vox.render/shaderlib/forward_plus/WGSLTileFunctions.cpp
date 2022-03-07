//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "WGSLTileFunctions.h"
#include <fmt/core.h>

namespace vox {
WGSLTileFunctions::WGSLTileFunctions(const std::array<uint32_t, 3>& tileCount):
_tileCount(tileCount) {
}

void WGSLTileFunctions::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(fmt::format("let tileCount : vec3<u32> = vec3<u32>({}u, {}u, ${});",
                                  _tileCount[0], _tileCount[1], _tileCount[2]));
    std::string tileFunc =
    "fn linearDepth(depthSample : f32) -> f32 {\n"
    "  return projection.zFar*projection.zNear / fma(depthSample, projection.zNear-projection.zFar, projection.zFar);\n"
    "}\n"
    "\n"
    "fn getTile(fragCoord : vec4<f32>) -> vec3<u32> {\n"
    "  // TODO: scale and bias calculation can be moved outside the shader to save cycles.\n"
    "  let sliceScale = f32(tileCount.z) / log2(projection.zFar / projection.zNear);\n"
    "  let sliceBias = -(f32(tileCount.z) * log2(projection.zNear) / log2(projection.zFar / projection.zNear));\n"
    "  let zTile = u32(max(log2(linearDepth(fragCoord.z)) * sliceScale + sliceBias, 0.0));\n"
    "\n"
    "  return vec3<u32>(u32(fragCoord.x / (projection.outputSize.x / f32(tileCount.x))),\n"
    "      u32(fragCoord.y / (projection.outputSize.y / f32(tileCount.y))),\n"
    "      zTile);\n"
    "}\n"
    "\n"
    "fn getClusterIndex(fragCoord : vec4<f32>) -> u32 {\n"
    "  let tile = getTile(fragCoord);\n"
    "  return tile.x +\n"
    "      tile.y * tileCount.x +\n"
    "      tile.z * tileCount.x * tileCount.y;\n"
    "}";
    encoder.addFunction(tileFunc);
}

}
