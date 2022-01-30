//
//  ktx_image.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/30.
//

#ifndef ktx_image_hpp
#define ktx_image_hpp

#include "image.h"

namespace vox {
class Ktx : public Image {
public:
    Ktx(const std::vector<uint8_t> &data);
    
    virtual ~Ktx() = default;
};

}        // namespace vox

#endif /* ktx_image_hpp */
