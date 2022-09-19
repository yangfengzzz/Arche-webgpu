//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpasses/geometry_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/material/material.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/resource_cache.h"

namespace vox {
GeometrySubpass::GeometrySubpass(RenderContext *renderContext,
                                 wgpu::TextureFormat depthStencilTextureFormat,
                                 Scene *scene,
                                 Camera *camera)
    : ForwardSubpass(renderContext, depthStencilTextureFormat, scene, camera) {}

void GeometrySubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder, ShaderVariant &variant) {
    opaqueQueue.clear();
    alphaTestQueue.clear();
    transparentQueue.clear();
    _callRender(_camera);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    for (const auto &element : opaqueQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (const auto &element : alphaTestQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (const auto &element : transparentQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
}

void GeometrySubpass::_callRender(Camera *camera) {
    const auto &_renderers = ComponentsManager::getSingleton()._renderers;
    for (auto &element : _renderers) {
        // filter by camera culling mask.
        if (!(camera->cullingMask & element->entity()->layer)) {
            continue;
        }

        // filter by camera frustum.
        if (camera->enableFrustumCulling) {
            element->isCulled = !camera->frustum().intersectsBox(element->bounds());
            if (element->isCulled) {
                continue;
            }
        }

        const auto &transform = camera->entity()->transform;
        const auto position = transform->worldPosition();
        auto center = element->bounds().midPoint();
        if (camera->isOrthographic()) {
            const auto forward = transform->worldForward();
            const auto offset = center - position;
            element->setDistanceForSort(offset.dot(forward));
        } else {
            element->setDistanceForSort(center.distanceSquaredTo(position));
        }

        element->render(opaqueQueue, alphaTestQueue, transparentQueue);
    }
}

}  // namespace vox
