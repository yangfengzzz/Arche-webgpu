//
//  forward_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/25.
//

#include "forward_subpass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "camera.h"
#include "renderer.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext* renderContext,
                               Scene* scene,
                               Camera* camera):
Subpass(renderContext, scene, camera) {
}

void ForwardSubpass::prepare() {
    {
        _bindGroupLayoutDescriptor.entryCount = 2;
        _bindGroupLayoutDescriptor.entries = _bindGroupLayoutEntries.data();
        _bindGroupLayoutEntries[0].binding = 0;
        _bindGroupLayoutEntries[0].visibility = wgpu::ShaderStage::Vertex;
        _bindGroupLayoutEntries[0].buffer.type = wgpu::BufferBindingType::Uniform;
        _bindGroupLayoutEntries[1].binding = 1;
        _bindGroupLayoutEntries[1].visibility = wgpu::ShaderStage::Vertex;
        _bindGroupLayoutEntries[1].buffer.type = wgpu::BufferBindingType::Uniform;
        _bindGroupLayout = _renderContext->device().CreateBindGroupLayout(&_bindGroupLayoutDescriptor);
    }
    {
        _bindGroupDescriptor.layout = _bindGroupLayout;
        _bindGroupDescriptor.entryCount = 2;
        _bindGroupDescriptor.entries = _bindGroupEntries.data();
        _bindGroupEntries[0].binding = 0;
        _bindGroupEntries[1].binding = 1;
    }
    _forwardPipelineDescriptor.label = "Forward Pipeline";
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _fragment.targets = &_colorTargetState;
    {
        _pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        _pipelineLayoutDescriptor.bindGroupLayouts = &_bindGroupLayout;
        _pipelineLayout = _renderContext->device().CreatePipelineLayout(&_pipelineLayoutDescriptor);
        
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _fragment.entryPoint = "main";
        _colorTargetState.format = _renderContext->drawableTextureFormat();
        _depthStencil.format = _renderContext->depthStencilTextureFormat();
    }
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
        auto& renderer = element.renderer;
        
        _bindGroupEntries[0].buffer = _camera->shaderData.getData("u_projMat").value();
        _bindGroupEntries[1].buffer = renderer->shaderData.getData("u_MVMat").value();
        auto uniformBindGroup = _renderContext->device().CreateBindGroup(&_bindGroupDescriptor);
        passEncoder.SetBindGroup(0, uniformBindGroup);
        
        auto& mesh = element.mesh;
        auto& subMesh = element.subMesh;
        _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
        _forwardPipelineDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
        _forwardPipelineDescriptor.primitive.topology = subMesh->topology();
        auto material = element.material;
        material->renderState.apply(&_colorTargetState, &_depthStencil,
                                    _forwardPipelineDescriptor, passEncoder, false);
        auto renderPipeline = _renderContext->device().CreateRenderPipeline(&_forwardPipelineDescriptor);
        passEncoder.SetPipeline(renderPipeline);
        
        for (uint32_t j = 0; j < mesh->vertexBufferBindings().size(); j++) {
            auto vertexBufferBinding =  mesh->vertexBufferBindings()[j];
            if (vertexBufferBinding) {
                passEncoder.SetVertexBuffer(j, mesh->vertexBufferBindings()[j]->buffer());
            }
        }
        auto indexBufferBinding = mesh->indexBufferBinding();
        if (indexBufferBinding) {
            passEncoder.SetIndexBuffer(mesh->indexBufferBinding()->buffer(), mesh->indexBufferBinding()->format());
        }
        
        passEncoder.DrawIndexed(subMesh->count(), 1, subMesh->start(), 0, 0);
    }
}


}
