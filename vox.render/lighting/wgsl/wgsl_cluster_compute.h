//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_cluster_compute_hpp
#define wgsl_cluster_compute_hpp

#include "wgsl_cluster_common.h"
#include "shaderlib/wgsl_cache.h"

namespace vox {
//MARK: - WGSLTileFunctions
class WGSLTileFunctions {
public:
    WGSLTileFunctions(const std::array<uint32_t, 3>& tileCount);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    std::array<uint32_t, 3> _tileCount{};
};

//MARK: - WGSLClusterStructs
class WGSLClusterStructs {
public:
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
};

//MARK: - WGSLClusterLightsStructs
class WGSLClusterLightsStructs {
public:
    WGSLClusterLightsStructs(uint32_t totalTiles, uint32_t maxLightsPerCluster);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    uint32_t _totalTiles;
    uint32_t _maxLightsPerCluster;
};

//MARK: - WGSLClusterBoundsSource
class WGSLClusterBoundsSource : public WGSLCache {
public:
    WGSLClusterBoundsSource(const std::array<uint32_t, 3>& tileCount, uint32_t maxLightsPerCluster,
                            const std::array<uint32_t, 3>& workgroupSize);
    
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
        
private:
    std::array<uint32_t, 3> _tileCount{};
    std::array<uint32_t, 3> _workgroupSize{};
    
    WGSLClusterStructs _clusterStructs;
    WGSLClusterLightsStructs _clusterLightsStructs;
};

//MARK: - WGSLClusterLightsSource
class WGSLClusterLightsSource : public WGSLCache {
public:
    WGSLClusterLightsSource(const std::array<uint32_t, 3>& tileCount, uint32_t maxLightsPerCluster,
                            const std::array<uint32_t, 3>& workgroupSize);
    
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
        
private:
    std::array<uint32_t, 3> _tileCount{};
    std::array<uint32_t, 3> _workgroupSize{};
    
    WGSLProjectionUniforms _projectionUniforms;
    WGSLViewUniforms _viewUniforms;
    WGSLLightUniforms _lightUniforms;
    WGSLClusterLightsStructs _clusterLightsStructs;
    WGSLClusterStructs _clusterStructs;
    WGSLTileFunctions _tileFunctions;
};

}
#endif /* wgsl_cluster_compute_hpp */
