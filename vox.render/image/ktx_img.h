//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/image.h"

namespace vox {
class Ktx : public Image {
public:
    Ktx(const std::string &name, const std::vector<uint8_t> &data, bool flipY);

    ~Ktx() override = default;
};

}  // namespace vox