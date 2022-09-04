//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLCommonFrag {
public:
    WGSLCommonFrag(const std::string& inputStructName);

    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);

private:
    std::string _inputStructName{};

    std::string _cameraStruct{};
    std::string _rendererStruct{};
};

}  // namespace vox
