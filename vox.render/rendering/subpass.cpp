//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "subpass.h"

namespace vox {
void Subpass::setRenderPass(RenderPass* pass) {
    _pass = pass;
    prepare();
}

}
