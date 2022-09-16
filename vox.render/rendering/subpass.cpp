//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpass.h"

#include "vox.render/material/material.h"
#include "vox.render/renderer.h"

namespace vox {
Subpass::Subpass(RenderContext* renderContext, Scene* scene, Camera* camera)
    : _renderContext(renderContext), _scene(scene), _camera(camera) {}

bool Subpass::_compareFromNearToFar(const RenderElement& a, const RenderElement& b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
           (a.renderer->distanceForSort() < b.renderer->distanceForSort());
}

bool Subpass::_compareFromFarToNear(const RenderElement& a, const RenderElement& b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
           (b.renderer->distanceForSort() < a.renderer->distanceForSort());
}

}  // namespace vox
