//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_subpass.h"
#include "material/material.h"
#include "renderer.h"
#include <glog/logging.h>

namespace vox {
Subpass::Type RenderSubpass::type() {
    return Subpass::Type::Render;
}

RenderSubpass::RenderSubpass(RenderContext* renderContext,
                             Scene* scene,
                             Camera* camera):
Subpass(renderContext, scene, camera) {
}

bool RenderSubpass::_compareFromNearToFar(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (a.renderer->distanceForSort() < b.renderer->distanceForSort());
}

bool RenderSubpass::_compareFromFarToNear(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (b.renderer->distanceForSort() < a.renderer->distanceForSort());
}

}
