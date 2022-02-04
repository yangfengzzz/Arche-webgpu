//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ktx_image_hpp
#define ktx_image_hpp

#include "image.h"

namespace vox {
class Ktx : public Image {
public:
    Ktx(const std::vector<uint8_t> &data, bool flipY);
    
    virtual ~Ktx() = default;
};

}        // namespace vox

#endif /* ktx_image_hpp */
