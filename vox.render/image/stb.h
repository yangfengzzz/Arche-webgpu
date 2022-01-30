//
//  stb.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/30.
//

#ifndef stb_hpp
#define stb_hpp

#include "image.h"

namespace vox {
class Stb : public Image {
public:
    Stb(const std::vector<uint8_t> &data);
    
    virtual ~Stb() = default;
};

}        // namespace vox
#endif /* stb_hpp */
