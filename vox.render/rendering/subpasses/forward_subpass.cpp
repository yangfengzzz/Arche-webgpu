//
//  forward_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#include "forward_subpass.h"
#include "material/material.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext* renderContext,
                               Scene* scene,
                               Camera* camera):
Subpass(renderContext, scene, camera) {
    _depthStencil = std::make_unique<wgpu::DepthStencilState>();
    _fragment = std::make_unique<wgpu::FragmentState>();
    _forwardPipelineDescriptor.depthStencil = _depthStencil.get();
    _forwardPipelineDescriptor.fragment = _fragment.get();
}

void ForwardSubpass::prepare() {
    
}

void ForwardSubpass::draw(wgpu::RenderPassEncoder& passEncoder) {
    passEncoder.PushDebugGroup("Draw Element");
    _drawMeshes(passEncoder);
    passEncoder.PopDebugGroup();
}

void ForwardSubpass::_drawMeshes(wgpu::RenderPassEncoder &passEncoder) {
//    auto compileMacros = ShaderMacroCollection();
//    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
//    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    _drawElement(passEncoder, opaqueQueue);
    _drawElement(passEncoder, alphaTestQueue);
    _drawElement(passEncoder, transparentQueue);
}

void ForwardSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                  const std::vector<RenderElement> &items) {
    for (auto &element : items) {
        auto material = element.material;
        material->renderState.apply(_fragment.get(), _depthStencil.get(),
                                    _forwardPipelineDescriptor, passEncoder, false);

    }
}


}
