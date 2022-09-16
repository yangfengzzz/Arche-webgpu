//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/texture.h"

namespace vox {
class Stb : public Image {
public:
    Stb(const std::vector<uint8_t> &data, bool flipY);

    ~Stb() override = default;
};

}  // namespace vox
