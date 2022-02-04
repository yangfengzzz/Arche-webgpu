//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_pbr_helper_hpp
#define wgsl_pbr_helper_hpp

#include "wgsl_encoder.h"
#include "wgsl_normal_get.h"

namespace vox {
class WGSLPbrHelper {
public:
    WGSLPbrHelper(const std::string& outputStructName, bool is_metallic_workflow);
    
    void operator()(WGSLEncoder& encoder,
                    const ShaderMacroCollection& macros, size_t counterIndex);
    
    void setParamName(const std::string& name);
    
    const std::string& paramName() const;
    
private:
    std::string _paramName{};
    std::string _outputStructName{};
    bool _is_metallic_workflow;
    WGSLNormalGet _normalGet;
};

}

#endif /* wgsl_pbr_helper_hpp */
