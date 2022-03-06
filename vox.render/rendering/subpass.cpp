//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "subpass.h"
#include "material/material.h"
#include "renderer.h"
#include <glog/logging.h>

namespace vox {
Subpass::Subpass(RenderContext* renderContext,
                 Scene* scene,
                 Camera* camera):
_renderContext(renderContext),
_scene(scene),
_camera(camera) {
}

void Subpass::setRenderPass(RenderPass* pass) {
    _pass = pass;
    prepare();
}

bool Subpass::_compareFromNearToFar(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (a.renderer->distanceForSort() < b.renderer->distanceForSort());
}

bool Subpass::_compareFromFarToNear(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (b.renderer->distanceForSort() < a.renderer->distanceForSort());
}

}
