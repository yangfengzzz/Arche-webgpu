//
//  stb.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/30.
//

#include "stb.h"
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include "std_helpers.h"

namespace vox {
Stb::Stb(const std::vector<uint8_t> &data) :
Image{} {
    int width;
    int height;
    int comp;
    int req_comp = 4;
    
    auto data_buffer = reinterpret_cast<const stbi_uc *>(data.data());
    auto data_size = static_cast<int>(data.size());
    
    auto raw_data = stbi_load_from_memory(data_buffer, data_size, &width, &height, &comp, req_comp);
    
    if (!raw_data) {
        throw std::runtime_error{std::string("Failed to load: ") + stbi_failure_reason()};
    }
    
    setData(raw_data, width * height * req_comp);
    stbi_image_free(raw_data);
    
    setFormat(wgpu::TextureFormat::RGBA8Unorm);
    setWidth(to_u32(width));
    setHeight(to_u32(height));
    setDepth(1u);
}

}        // namespace vox
