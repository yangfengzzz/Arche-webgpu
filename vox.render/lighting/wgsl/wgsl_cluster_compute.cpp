//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_cluster_compute.h"

#include <spdlog/fmt/fmt.h>

#include <unordered_map>

namespace vox {
// MARK: - WGSLTileFunctions
WGSLTileFunctions::WGSLTileFunctions(const std::array<uint32_t, 3>& tileCount) : _tileCount(tileCount) {}

void WGSLTileFunctions::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(fmt::format("let tileCount : vec3<u32> = vec3<u32>({}u, {}u, {}u);\n", _tileCount[0],
                                  _tileCount[1], _tileCount[2]));
    std::string tileFunc =
            "fn linearDepth(depthSample : f32) -> f32 {\n"
            "  return u_cluster_uniform.zFar * u_cluster_uniform.zNear / fma(depthSample, u_cluster_uniform.zNear - "
            "u_cluster_uniform.zFar, u_cluster_uniform.zFar);\n"
            "}\n"
            "\n"
            "fn getTile(fragCoord : vec4<f32>) -> vec3<u32> {\n"
            "  // TODO: scale and bias calculation can be moved outside the shader to save cycles.\n"
            "  let sliceScale = f32(tileCount.z) / log2(u_cluster_uniform.zFar / u_cluster_uniform.zNear);\n"
            "  let sliceBias = -(f32(tileCount.z) * log2(u_cluster_uniform.zNear) / log2(u_cluster_uniform.zFar / "
            "u_cluster_uniform.zNear));\n"
            "  let zTile = u32(max(log2(linearDepth(fragCoord.z)) * sliceScale + sliceBias, 0.0));\n"
            "\n"
            "  return vec3<u32>(u32(fragCoord.x / (u_cluster_uniform.outputSize.x / f32(tileCount.x))),\n"
            "      u32(fragCoord.y / (u_cluster_uniform.outputSize.y / f32(tileCount.y))),\n"
            "      zTile);\n"
            "}\n"
            "\n"
            "fn getClusterIndex(fragCoord : vec4<f32>) -> u32 {\n"
            "  let tile = getTile(fragCoord);\n"
            "  return tile.x +\n"
            "      tile.y * tileCount.x +\n"
            "      tile.z * tileCount.x * tileCount.y;\n"
            "}\n";
    encoder.addFunction(tileFunc);
}

// MARK: - WGSLClusterStructs
WGSLClusterStructs::WGSLClusterStructs(uint32_t totalTiles) : _totalTiles(totalTiles) {}

void WGSLClusterStructs::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(
            fmt::format("struct ClusterBounds {{\n"
                        "  minAABB : vec3<f32>;\n"
                        "  maxAABB : vec3<f32>;\n"
                        "}};\n"
                        "struct Clusters {{\n"
                        "  bounds : array<ClusterBounds, {}>;\n"
                        "}};\n",
                        _totalTiles));
}

// MARK: - WGSLClusterLightsStructs
WGSLClusterLightsStructs::WGSLClusterLightsStructs(uint32_t totalTiles, uint32_t maxLightsPerCluster)
    : _totalTiles(totalTiles), _maxLightsPerCluster(maxLightsPerCluster) {}

void WGSLClusterLightsStructs::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    encoder.addStruct(
            fmt::format("struct ClusterLights {{\n"
                        "  offset : u32;\n"
                        "  point_count : u32;\n"
                        "  spot_count : u32;\n"
                        "}};\n"
                        "struct ClusterLightGroup {{\n"
                        "  offset : atomic<u32>;\n"
                        "  lights : array<ClusterLights, {}>;\n"
                        "  indices : array<u32, {}>;\n"
                        "}};\n",
                        _totalTiles, _totalTiles * _maxLightsPerCluster));
    encoder.addStorageBufferBinding("u_clusterLights", "ClusterLightGroup", false);
}

// MARK: - WGSLClusterBoundsSource
WGSLClusterBoundsSource::WGSLClusterBoundsSource(const std::array<uint32_t, 3>& tileCount,
                                                 uint32_t maxLightsPerCluster,
                                                 const std::array<uint32_t, 3>& workgroupSize)
    : _forwardPlusUniforms(),
      _clusterStructs(tileCount[0] * tileCount[1] * tileCount[2]),
      _tileCount(tileCount),
      _workgroupSize(workgroupSize) {}

void WGSLClusterBoundsSource::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
        _forwardPlusUniforms(encoder, macros);
        _clusterStructs(encoder, macros);
        encoder.addStorageBufferBinding("u_clusters", "Clusters", false);
        encoder.addFunction(
                "fn lineIntersectionToZPlane(a : vec3<f32>, b : vec3<f32>, zDistance : f32) -> vec3<f32> {\n"
                "  let normal = vec3<f32>(0.0, 0.0, 1.0);\n"
                "  let ab = b - a;\n"
                "  let t = (zDistance - dot(normal, a)) / dot(normal, ab);\n"
                "  return a + t * ab;\n"
                "}\n"
                "\n"
                "fn clipToView(clip : vec4<f32>) -> vec4<f32> {\n"
                "  let view = u_cluster_uniform.inverseMatrix * clip;\n"
                "  return view / vec4<f32>(view.w, view.w, view.w, view.w);\n"
                "}\n"
                "\n"
                "fn screen2View(screen : vec4<f32>) -> vec4<f32> {\n"
                "  let texCoord = screen.xy / u_cluster_uniform.outputSize.xy;\n"
                "  let clip = vec4<f32>(vec2<f32>(texCoord.x, 1.0 - texCoord.y) * 2.0 - vec2<f32>(1.0, 1.0), screen.z, "
                "screen.w);\n"
                "  return clipToView(clip);\n"
                "}\n");
        encoder.addFunction(fmt::format("let tileCount = vec3<u32>({}u, {}u, {}u);\n", _tileCount[0], _tileCount[1],
                                        _tileCount[2]));
        encoder.addFunction("let eyePos = vec3<f32>(0.0, 0.0, 0.0);\n");

        encoder.addEntry({_workgroupSize[0], _workgroupSize[1], _workgroupSize[2]},
                         [&](std::string& source) {
                             source +=
                                     "let tileIndex = global_id.x +\n"
                                     "    global_id.y * tileCount.x +\n"
                                     "    global_id.z * tileCount.x * tileCount.y;\n"
                                     "\n"
                                     "let tileSize = vec2<f32>(u_cluster_uniform.outputSize.x / f32(tileCount.x),\n"
                                     "u_cluster_uniform.outputSize.y / f32(tileCount.y));\n"
                                     "\n"
                                     "let maxPoint_sS = vec4<f32>(vec2<f32>(f32(global_id.x+1u), f32(global_id.y+1u)) "
                                     "* tileSize, 0.0, 1.0);\n"
                                     "let minPoint_sS = vec4<f32>(vec2<f32>(f32(global_id.x), f32(global_id.y)) * "
                                     "tileSize, 0.0, 1.0);\n"
                                     "\n"
                                     "let maxPoint_vS = screen2View(maxPoint_sS).xyz;\n"
                                     "let minPoint_vS = screen2View(minPoint_sS).xyz;\n"
                                     "\n"
                                     "let tileNear = -u_cluster_uniform.zNear * pow(u_cluster_uniform.zFar/ "
                                     "u_cluster_uniform.zNear, f32(global_id.z)/f32(tileCount.z));\n"
                                     "let tileFar = -u_cluster_uniform.zNear * pow(u_cluster_uniform.zFar/ "
                                     "u_cluster_uniform.zNear, f32(global_id.z+1u)/f32(tileCount.z));\n"
                                     "\n"
                                     "let minPointNear = lineIntersectionToZPlane(eyePos, minPoint_vS, tileNear);\n"
                                     "let minPointFar = lineIntersectionToZPlane(eyePos, minPoint_vS, tileFar);\n"
                                     "let maxPointNear = lineIntersectionToZPlane(eyePos, maxPoint_vS, tileNear);\n"
                                     "let maxPointFar = lineIntersectionToZPlane(eyePos, maxPoint_vS, tileFar);\n"
                                     "\n"
                                     "u_clusters.bounds[tileIndex].minAABB = min(min(minPointNear, "
                                     "minPointFar),min(maxPointNear, maxPointFar));\n"
                                     "u_clusters.bounds[tileIndex].maxAABB = max(max(minPointNear, "
                                     "minPointFar),max(maxPointNear, maxPointFar));\n";
                         },
                         {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

// MARK: - WGSLClusterLightsSource
WGSLClusterLightsSource::WGSLClusterLightsSource(const std::array<uint32_t, 3>& tileCount,
                                                 uint32_t maxLightsPerCluster,
                                                 const std::array<uint32_t, 3>& workgroupSize)
    : _forwardPlusUniforms(),
      _lightFragDefine(),
      _clusterLightsStructs(tileCount[0] * tileCount[1] * tileCount[2], maxLightsPerCluster),
      _clusterStructs(tileCount[0] * tileCount[1] * tileCount[2]),
      _tileFunctions(tileCount),
      _maxLightsPerCluster(maxLightsPerCluster),
      _tileCount(tileCount),
      _workgroupSize(workgroupSize) {}

void WGSLClusterLightsSource::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
        _forwardPlusUniforms(encoder, macros);
        _lightFragDefine(encoder, macros);
        _clusterLightsStructs(encoder, macros);
        _clusterStructs(encoder, macros);
        encoder.addStorageBufferBinding("u_clusters", "Clusters", false);
        _tileFunctions(encoder, macros);

        encoder.addFunction(
                "fn sqDistPointAABB(point : vec3<f32>, minAABB : vec3<f32>, maxAABB : vec3<f32>) -> f32 {\n"
                "  var sqDist = 0.0;\n"
                "  // const minAABB : vec3<f32> = u_clusters.bounds[tileIndex].minAABB;\n"
                "  // const maxAABB : vec3<f32> = u_clusters.bounds[tileIndex].maxAABB;\n"
                "\n"
                "  // Wait, does this actually work? Just porting code, but it seems suspect?\n"
                "  for(var i = 0; i < 3; i = i + 1) {\n"
                "    let v = point[i];\n"
                "    if(v < minAABB[i]){\n"
                "      sqDist = sqDist + (minAABB[i] - v) * (minAABB[i] - v);\n"
                "    }\n"
                "    if(v > maxAABB[i]){\n"
                "      sqDist = sqDist + (v - maxAABB[i]) * (v - maxAABB[i]);\n"
                "    }\n"
                "  }\n"
                "\n"
                "  return sqDist;\n"
                "}\n");

        encoder.addEntry(
                {_workgroupSize[0], _workgroupSize[1], _workgroupSize[2]},
                [&](std::string& source) {
                    source +=
                            "let tileIndex = global_id.x +\n"
                            "    global_id.y * tileCount.x +\n"
                            "    global_id.z * tileCount.x * tileCount.y;\n"
                            "\n"
                            "var clusterLightCount = 0u;\n";
                    source += fmt::format("var cluserLightIndices : array<u32, {}>;\n", _maxLightsPerCluster);
                    if (macros.contains(POINT_LIGHT_COUNT)) {
                        source += fmt::format("for (var i = 0u; i < {}u; i = i + 1u) {{\n",
                                              (int)*macros.macroConstant(POINT_LIGHT_COUNT));
                        source +=
                                "  let range = u_pointLight[i].distance;\n"
                                "  // Lights without an explicit range affect every cluster, but this is a poor way to "
                                "handle that.\n"
                                "  var lightInCluster = range <= 0.0;\n"
                                "\n"
                                "  if (!lightInCluster) {\n"
                                "    let lightViewPos = u_cluster_uniform.viewMatrix * "
                                "vec4<f32>(u_pointLight[i].position, 1.0);\n"
                                "    let sqDist = sqDistPointAABB(lightViewPos.xyz, "
                                "u_clusters.bounds[tileIndex].minAABB, u_clusters.bounds[tileIndex].maxAABB);\n"
                                "    lightInCluster = sqDist <= (range * range);\n"
                                "  }\n"
                                "\n"
                                "  if (lightInCluster) {\n"
                                "    // Light affects this cluster. Add it to the list.\n"
                                "    cluserLightIndices[clusterLightCount] = i;\n"
                                "    clusterLightCount = clusterLightCount + 1u;\n"
                                "  }\n"
                                "\n";
                        source += fmt::format("  if (clusterLightCount == {}u) {{\n", _maxLightsPerCluster);
                        source +=
                                "    break;\n"
                                "  }\n"
                                "}\n";
                    }

                    source += "let pointLightCount = clusterLightCount;\n";

                    if (macros.contains(SPOT_LIGHT_COUNT)) {
                        source += fmt::format("for (var i = 0u; i < {}u; i = i + 1u) {{\n",
                                              (int)*macros.macroConstant(SPOT_LIGHT_COUNT));
                        source +=
                                "  let range = u_spotLight[i].distance;\n"
                                "  // Lights without an explicit range affect every cluster, but this is a poor way to "
                                "handle that.\n"
                                "  var lightInCluster = range <= 0.0;\n"
                                "\n"
                                "  if (!lightInCluster) {\n"
                                "    let lightViewPos = u_cluster_uniform.viewMatrix * "
                                "vec4<f32>(u_spotLight[i].position, 1.0);\n"
                                "    let sqDist = sqDistPointAABB(lightViewPos.xyz, "
                                "u_clusters.bounds[tileIndex].minAABB, u_clusters.bounds[tileIndex].maxAABB);\n"
                                "    lightInCluster = sqDist <= (range * range);\n"
                                "  }\n"
                                "\n"
                                "  if (lightInCluster) {\n"
                                "    // Light affects this cluster. Add it to the list.\n"
                                "    cluserLightIndices[clusterLightCount] = i;\n"
                                "    clusterLightCount = clusterLightCount + 1u;\n"
                                "  }\n"
                                "\n";
                        source += fmt::format("  if (clusterLightCount == {}u) {{\n", _maxLightsPerCluster);
                        source +=
                                "    break;\n"
                                "  }\n"
                                "}\n";
                    }
                    source +=
                            "\n"
                            "var offset = atomicAdd(&u_clusterLights.offset, clusterLightCount);\n"
                            "\n"
                            "for(var i = 0u; i < clusterLightCount; i = i + 1u) {\n"
                            "  u_clusterLights.indices[offset + i] = cluserLightIndices[i];\n"
                            "}\n"
                            "u_clusterLights.lights[tileIndex].offset = offset;\n"
                            "u_clusterLights.lights[tileIndex].point_count = pointLightCount;\n"
                            "u_clusterLights.lights[tileIndex].spot_count = clusterLightCount - pointLightCount;\n";
                },
                {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}  // namespace vox
