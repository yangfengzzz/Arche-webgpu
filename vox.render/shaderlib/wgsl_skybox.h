//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_skybox_hpp
#define wgsl_skybox_hpp

#include "wgsl_cache.h"
#include "functors/wgsl_common.h"
#include "functors/wgsl_common_vert.h"

namespace vox {
//MARK: - Skybox Vertex Code
class WGSLSkyboxVertex : public WGSLCache {
public:
    WGSLSkyboxVertex();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommonVert _commonVert;
};

//MARK: - Skybox Fragment Code
class WGSLSkyboxFragment : public WGSLCache {
public:
    WGSLSkyboxFragment();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommon _common;
};

}
#endif /* wgsl_skybox_hpp */
