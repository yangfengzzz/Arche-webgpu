//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_common_vert_hpp
#define wgsl_common_vert_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLCommonVert {
public:
    WGSLCommonVert(const std::string& inputStructName);

    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);

private:
    std::string _inputStructName{};

    std::string _cameraStruct{};
    std::string _rendererStruct{};
};

}  // namespace vox

#endif /* wgsl_common_vert_hpp */
