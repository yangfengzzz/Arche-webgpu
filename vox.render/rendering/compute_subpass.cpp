//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_subpass.h"

namespace vox {
Subpass::Type ComputeSubpass::type() {
    return Subpass::Type::Compute;
}

ComputeSubpass::ComputeSubpass(RenderContext *context,
                               Scene *scene,
                               Camera *camera) :
Subpass(context, scene, camera) {
}

}
