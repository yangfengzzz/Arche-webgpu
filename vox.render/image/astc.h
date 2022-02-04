//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef astc_hpp
#define astc_hpp

#include "image.h"

namespace vox {
struct BlockDim {
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

class Astc : public Image {
public:
    /**
     * @brief Decodes an ASTC image
     * @param image Image to decode
     */
    Astc(const Image &image, bool flipY);
    
    /**
     * @brief Decodes ASTC data with an ASTC header
     * @param data ASTC data with header
     */
    Astc(const std::vector<uint8_t> &data, bool flipY);
    
    virtual ~Astc() = default;
    
private:
    /**
     * @brief Decodes ASTC data
     * @param blockdim Dimensions of the block
     * @param extent Extent of the image
     * @param data Pointer to ASTC image data
     */
    void decode(BlockDim blockdim, wgpu::Extent3D extent, const uint8_t *data);
    
    /**
     * @brief Initializes ASTC library
     */
    void init();
};

}        // namespace vox

#endif /* astc_hpp */
