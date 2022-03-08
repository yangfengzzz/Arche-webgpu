//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_quad_hpp
#define wgsl_quad_hpp

#include "wgsl_cache.h"

namespace vox {
//MARK: - Quad Vertex Code
class WGSLQuadVertex : public WGSLCache {
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
};

}
#endif /* wgsl_quad_hpp */
