//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_grid_hpp
#define wgsl_grid_hpp

#include "shaderlib/wgsl_cache.h"
#include "shaderlib/functors/wgsl_common_vert.h"

namespace vox {
//MARK: - Unlit Vertex Code
class WGSLGridVertex : public WGSLCache {
public:
    WGSLGridVertex();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    WGSLCommonVert _commonVert;
};

//MARK: - Unlit Fragment Code
class WGSLGridFragment : public WGSLCache {
public:
    WGSLGridFragment();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
};

}

#endif /* wgsl_grid_hpp */
