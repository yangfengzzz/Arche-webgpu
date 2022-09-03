//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/helper.h"

namespace vox {
/**
 * @brief Adds support for C style preprocessor macros to glsl shaders
 *        enabling you to define or undefine certain symbols
 */
class ShaderVariant {
public:
    ShaderVariant() = default;

    ShaderVariant(std::string &&preamble, std::vector<std::string> &&processes);

    [[nodiscard]] size_t GetId() const;

    /**
     * Union of two variant collection.
     * @param left - input variant collection
     * @param right - input variant collection
     * @param result - union variant macro collection
     */
    static void UnionCollection(const ShaderVariant &left, const ShaderVariant &right, ShaderVariant &result);

    /**
     * @brief Adds a define macro to the shader
     * @param def String which should go to the right of a define directive
     */
    void AddDefine(const std::string &def);

    /**
     * @brief Remove a def macro to the shader
     * @param def String which should go to the right of a define directive
     */
    void RemoveDefine(const std::string &def);

    /**
     * @brief Specifies the size of a named runtime array for automatic reflection. If already specified, overrides the
     * size.
     * @param runtime_array_name String under which the runtime array is named in the shader
     * @param size Integer specifying the wanted size of the runtime array (in number of elements, not size in bytes),
     * used for automatic allocation of buffers. See get_declared_struct_size_runtime_array() in spirv_cross.h
     */
    void AddRuntimeArraySize(const std::string &runtime_array_name, size_t size);

    void SetRuntimeArraySizes(const std::unordered_map<std::string, size_t> &sizes);

    [[nodiscard]] std::string GetPreamble() const;

    [[nodiscard]] const std::vector<std::string> &GetProcesses() const;

    [[nodiscard]] const std::unordered_map<std::string, size_t> &GetRuntimeArraySizes() const;

    void Clear();

private:
    size_t id_{};

    std::set<std::string> preambles_;

    std::vector<std::string> processes_;

    std::unordered_map<std::string, size_t> runtime_array_sizes_;

    void UpdateId();
};

}  // namespace vox
