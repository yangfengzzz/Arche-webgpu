//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/texture.h"
#include "vox.render/vk_common.h"

namespace vox {
struct BlockDim {
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

class Astc : public Texture {
public:
    /**
     * @brief Decodes an ASTC image
     * @param image Image to decode
     */
    explicit Astc(const Texture &image);

    /**
     * @brief Decodes ASTC data with an ASTC header
     * @param name Name of the component
     * @param data ASTC data with header
     */
    Astc(const std::string &name, const std::vector<uint8_t> &data);

    ~Astc() override = default;

private:
    /**
     * @brief Decodes ASTC data
     * @param blockdim Dimensions of the block
     * @param extent Extent of the image
     * @param data Pointer to ASTC image data
     */
    void Decode(BlockDim blockdim, VkExtent3D extent, const uint8_t *data);

    /**
     * @brief Initializes ASTC library
     */
    static void Init();
};

}  // namespace vox
