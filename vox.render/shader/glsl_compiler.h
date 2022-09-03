//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <glslang/Public/ShaderLang.h>

#include <string>
#include <vector>

#include "vox.render/shader/shader_module.h"

namespace vox {
/// Helper class to generate SPIRV code from GLSL source
/// A very simple version of the glslValidator application
class GLSLCompiler {
private:
    static glslang::EShTargetLanguage env_target_language_;
    static glslang::EShTargetLanguageVersion env_target_language_version_;

public:
    /**
     * @brief Set the glslang target environment to translate to when generating code
     * @param target_language The language to translate to
     * @param target_language_version The version of the language to translate to
     */
    static void SetTargetEnvironment(glslang::EShTargetLanguage target_language,
                                     glslang::EShTargetLanguageVersion target_language_version);

    /**
     * @brief Reset the glslang target environment to the default values
     */
    static void ResetTargetEnvironment();

    /**
     * @brief Compiles GLSL to SPIRV code
     * @param stage The Vulkan shader stage flag
     * @param glsl_source The GLSL source code to be compiled
     * @param entry_point The entrypoint function name of the shader stage
     * @param shader_variant The shader variant
     * @param[out] spirv The generated SPIRV code
     * @param[out] info_log Stores any log messages during the compilation process
     */
    static bool CompileToSpirv(wgpu::ShaderStage stage,
                               const std::vector<uint8_t> &glsl_source,
                               const std::string &entry_point,
                               const ShaderVariant &shader_variant,
                               std::vector<std::uint32_t> &spirv,
                               std::string &info_log);
};

}  // namespace vox
