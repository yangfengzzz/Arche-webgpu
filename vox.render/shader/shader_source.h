//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/helper.h"

namespace vox {
class ShaderSource {
public:
    ShaderSource() = default;

    explicit ShaderSource(const std::string &filename);

    [[nodiscard]] size_t GetId() const;

    [[nodiscard]] const std::string &GetFilename() const;

    void SetSource(const std::string &source);

    [[nodiscard]] const std::string &GetSource() const;

private:
    size_t id_{};

    std::string filename_;

    std::string source_;
};

}  // namespace vox
