//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_source.h"

#include "vox.render/platform/filesystem.h"

namespace vox {
ShaderSource::ShaderSource(const std::string &filename) : filename_{filename}, source_{fs::ReadShader(filename)} {
    std::hash<std::string> hasher{};
    id_ = hasher(std::string{source_.cbegin(), source_.cend()});
}

size_t ShaderSource::GetId() const { return id_; }

const std::string &ShaderSource::GetFilename() const { return filename_; }

void ShaderSource::SetSource(const std::string &source) {
    source_ = source;
    std::hash<std::string> hasher{};
    id_ = hasher(std::string{source_.cbegin(), source_.cend()});
}

const std::string &ShaderSource::GetSource() const { return source_; }

}  // namespace vox
