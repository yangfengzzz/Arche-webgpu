//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLPbrFragDefine {
public:
    WGSLPbrFragDefine(const std::string& outputStructName, bool is_metallic_workflow);

    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros, size_t counterIndex);

private:
    std::string _outputStructName{};
    bool _is_metallic_workflow;
    std::string _pbrStruct;
};

}  // namespace vox
