//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef WGSLTileFunctions_hpp
#define WGSLTileFunctions_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLTileFunctions {
public:
    WGSLTileFunctions(const std::array<uint32_t, 3>& tileCount);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    std::array<uint32_t, 3> _tileCount{};
};

}
#endif /* WGSLTileFunctions_hpp */
