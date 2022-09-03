//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/stb_tex.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#include <stb_image.h>

namespace vox {
Stb::Stb(const std::string &name, const std::vector<uint8_t> &data) : Texture{name} {
    int width;
    int height;
    int comp;
    int req_comp = 4;

    auto data_buffer = reinterpret_cast<const stbi_uc *>(data.data());
    auto data_size = static_cast<int>(data.size());

    auto raw_data = stbi_load_from_memory(data_buffer, data_size, &width, &height, &comp, req_comp);

    if (!raw_data) {
        throw std::runtime_error{"Failed to load " + name + ": " + stbi_failure_reason()};
    }

    SetData(raw_data, width * height * req_comp);
    stbi_image_free(raw_data);

    SetFormat(VK_FORMAT_R8G8B8A8_UNORM);
    SetWidth(utility::ToU32(width));
    SetHeight(utility::ToU32(height));
    SetDepth(1u);
}

}  // namespace vox
