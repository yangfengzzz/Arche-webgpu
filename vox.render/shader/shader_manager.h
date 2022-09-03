//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/singleton.h"
#include "vox.render/shader/shader_source.h"

namespace vox {
class ShaderManager : public Singleton<ShaderManager> {
public:
    static ShaderManager &GetSingleton();

    static ShaderManager *GetSingletonPtr();

    /**
     * @brief Loads shader source
     */
    std::shared_ptr<ShaderSource> LoadShader(const std::string &file);

    void CollectGarbage();

private:
    std::unordered_map<std::string, std::shared_ptr<ShaderSource>> shader_pool_;
};

template <>
inline ShaderManager *Singleton<ShaderManager>::ms_singleton{nullptr};

}  // namespace vox
