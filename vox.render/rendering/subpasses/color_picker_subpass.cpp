//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color_picker_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "renderer.h"
#include "camera.h"

namespace vox {
ColorPickerSubpass::ColorPickerSubpass(RenderContext* renderContext,
                                       Scene* scene,
                                       Camera* camera):
Subpass(renderContext, scene, camera) {
}

void ColorPickerSubpass::prepare() {
    _forwardPipelineDescriptor.label("ColorPicker Pipeline");
    _forwardPipelineDescriptor.colorAttachments[0].pixelFormat(_pass->renderPassDescriptor()->colorAttachments[0].texture().pixelFormat());
    _forwardPipelineDescriptor.depthAttachmentPixelFormat(_view->depthStencilPixelFormat());
    
    MTL::DepthStencilDescriptor depthStencilDesc;
    depthStencilDesc.label("ColorPicker Creation");
    depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
    depthStencilDesc.depthWriteEnabled(true);
    _forwardDepthStencilState = _view->device().makeDepthStencilState(depthStencilDesc);
}

void ColorPickerSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    _currentId = 0;
    _primitivesMap.clear();
    
    commandEncoder.pushDebugGroup("Draw ColorPicker");
    commandEncoder.setCullMode(MTL::CullModeFront);
    commandEncoder.setDepthStencilState(_forwardDepthStencilState);
    commandEncoder.setStencilReferenceValue(128);
    
    _drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ColorPickerSubpass::_drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    _drawElement(renderEncoder, opaqueQueue, compileMacros);
    _drawElement(renderEncoder, alphaTestQueue, compileMacros);
    _drawElement(renderEncoder, transparentQueue, compileMacros);
}

void ColorPickerSubpass::_drawElement(MTL::RenderCommandEncoder &renderEncoder,
                                      const std::vector<RenderElement> &items,
                                      const ShaderMacroCollection& compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto renderer = element.renderer;
        renderer->shaderData.mergeMacro(macros, macros);
        
        auto material = element.material;
        material->shaderData.mergeMacro(macros, macros);
        ShaderProgram *program = _pass->resourceCache().requestShader(_pass->library(), "vertex_picker",
                                                                      "fragment_picker", macros);
        if (!program->isValid()) {
            continue;
        }
        _forwardPipelineDescriptor.vertexFunction(program->vertexShader());
        _forwardPipelineDescriptor.fragmentFunction(program->fragmentShader());
        
        // manully
        auto& mesh = element.mesh;
        _forwardPipelineDescriptor.vertexDescriptor(&mesh->vertexDescriptor());
        
        auto m_forwardPipelineState = _pass->resourceCache().requestRenderPipelineState(_forwardPipelineDescriptor);
        uploadUniforms(renderEncoder, m_forwardPipelineState->materialUniformBlock, material->shaderData);
        uploadUniforms(renderEncoder, m_forwardPipelineState->rendererUniformBlock, renderer->shaderData);
        uploadUniforms(renderEncoder, m_forwardPipelineState->sceneUniformBlock, _scene->shaderData);
        uploadUniforms(renderEncoder, m_forwardPipelineState->cameraUniformBlock, _camera->shaderData);
        renderEncoder.setRenderPipelineState(*m_forwardPipelineState);
        
        _currentId += 1;
        _primitivesMap[_currentId] = std::make_pair(renderer, mesh);
        Vector3F color = id2Color(_currentId);
        renderEncoder.setFragmentBytes(&color, sizeof(Vector4F), 0);
        
        for (auto &meshBuffer: mesh->vertexBuffers()) {
            renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                          meshBuffer.offset(),
                                          meshBuffer.argumentIndex());
        }
        auto& submesh = element.subMesh;
        renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                            submesh->indexCount(),
                                            submesh->indexType(),
                                            submesh->indexBuffer().buffer(),
                                            submesh->indexBuffer().offset());
    }
}

//MARK: - Picker
Vector3F ColorPickerSubpass::id2Color(uint32_t id) {
    if (id >= 0xffffff) {
        std::cout << "Framebuffer Picker encounter primitive's id greater than " + std::to_string(0xffffff)
        << std::endl;
        return Vector3F(0, 0, 0);
    }
    
    return Vector3F((id & 0xff) / 255.0, ((id & 0xff00) >> 8) / 255.0, ((id & 0xff0000) >> 16) / 255.0);
}

uint32_t ColorPickerSubpass::color2Id(const std::array<uint8_t, 4> &color) {
    return color[0] | (color[1] << 8) | (color[2] << 16);
}

std::pair<Renderer *, MeshPtr> ColorPickerSubpass::getObjectByColor(const std::array<uint8_t, 4> &color) {
    auto iter = _primitivesMap.find(color2Id(color));
    if (iter != _primitivesMap.end()) {
        return iter->second;
    } else {
        return std::make_pair(nullptr, nullptr);
    }
}

}
