//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpasses/color_picker_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/rendering/resource_cache.h"

namespace vox {
ColorPickerSubpass::ColorPickerSubpass(RenderContext *renderContext,
                                       wgpu::TextureFormat depthStencilTextureFormat,
                                       Scene *scene,
                                       Camera *camera)
    : Subpass(renderContext, scene, camera), _depthStencilTextureFormat(depthStencilTextureFormat) {
    _material = std::make_shared<UnlitMaterial>(renderContext->device());
}

void ColorPickerSubpass::prepare() {
    _depthStencil.format = _depthStencilTextureFormat;
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;
    _colorTargetState.format = wgpu::TextureFormat::BGRA8Unorm;
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "ColorPicker Pipeline";
    {
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _fragment.entryPoint = "main";
    }
}

void ColorPickerSubpass::draw(wgpu::RenderPassEncoder &passEncoder) {
    _currentId = 0;
    _primitivesMap.clear();

    passEncoder.PushDebugGroup("Draw ColorPicker");
    _drawMeshes(passEncoder);
    passEncoder.PopDebugGroup();
}

void ColorPickerSubpass::_drawMeshes(wgpu::RenderPassEncoder &passEncoder) {
    auto compile_variant = ShaderVariant();
    _scene->shaderData.mergeVariants(compile_variant, compile_variant);
    if (_camera) {
        _camera->shaderData.mergeVariants(compile_variant, compile_variant);
    }

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    ComponentsManager::getSingleton().callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    // prealloc buffer
    size_t total = opaqueQueue.size() + alphaTestQueue.size() + transparentQueue.size();
    _bufferPool.reserve(total);
    for (size_t i = _bufferPool.size(); i < total; i++) {
        _bufferPool.emplace_back(_renderContext->device(), sizeof(Color),
                                 wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst);
    }

    _drawElement(passEncoder, opaqueQueue, compile_variant);
    _drawElement(passEncoder, alphaTestQueue, compile_variant);
    _drawElement(passEncoder, transparentQueue, compile_variant);
}

void ColorPickerSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                      const std::vector<RenderElement> &items,
                                      const ShaderVariant &variant) {
    for (auto &element : items) {
        auto macros = variant;
        auto &renderer = element.renderer;
        renderer->updateShaderData();
        renderer->shaderData.mergeVariants(macros, macros);
        auto &mesh = element.mesh;
        auto &subMesh = element.subMesh;

        _primitivesMap[_currentId] = std::make_pair(renderer, mesh);
        Vector3F color = id2Color(_currentId);
        auto reverseColor = Color(color.z, color.y, color.x, 1);
        Buffer &buffer = _bufferPool[_currentId];
        buffer.uploadData(_renderContext->device(), &reverseColor, sizeof(Color));
        _currentId += 1;

        // PSO
        {
            auto &vert_shader_module = ResourceCache::GetSingleton().requestShaderModule(
                    wgpu::ShaderStage::Vertex, *_material->vertex_source_, macros);
            auto &frag_shader_module = ResourceCache::GetSingleton().requestShaderModule(
                    wgpu::ShaderStage::Fragment, *_material->fragment_source_, macros);
            _forwardPipelineDescriptor.vertex.module = vert_shader_module.handle();
            _fragment.module = frag_shader_module.handle();

            _bindGroupLayoutEntryVecMap.clear();
            _bindGroupEntryVecMap.clear();
            _scene->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                        _bindGroupEntryVecMap);
            _scene->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                        _bindGroupEntryVecMap);
            _camera->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                         _bindGroupEntryVecMap);
            _camera->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                         _bindGroupEntryVecMap);
            renderer->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                          _bindGroupEntryVecMap);
            renderer->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                          _bindGroupEntryVecMap);
            _material->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                           _bindGroupEntryVecMap);
            _material->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                           _bindGroupEntryVecMap);

            std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
            for (const auto &bindGroupLayoutEntryVec : _bindGroupLayoutEntryVecMap) {
                bindGroupLayoutDescriptor.entries = bindGroupLayoutEntryVec.second.data();
                bindGroupLayoutDescriptor.entryCount = bindGroupLayoutEntryVec.second.size();
                wgpu::BindGroupLayout bindGroupLayout =
                        ResourceCache::GetSingleton().requestBindGroupLayout(bindGroupLayoutDescriptor);

                const auto group = bindGroupLayoutEntryVec.first;
                const auto &bindGroupEntryVec = _bindGroupEntryVecMap[group];
                _bindGroupDescriptor.layout = bindGroupLayout;
                _bindGroupDescriptor.entryCount = bindGroupEntryVec.size();
                _bindGroupDescriptor.entries = bindGroupEntryVec.data();
                auto uniformBindGroup = ResourceCache::GetSingleton().requestBindGroup(_bindGroupDescriptor);
                passEncoder.SetBindGroup(group, uniformBindGroup);
                bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
            }

            _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
            _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
            _pipelineLayout = ResourceCache::GetSingleton().requestPipelineLayout(_pipelineLayoutDescriptor);
            _forwardPipelineDescriptor.layout = _pipelineLayout;

            _material->renderState.apply(&_colorTargetState, &_depthStencil, _forwardPipelineDescriptor, passEncoder,
                                         true);

            _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
            _forwardPipelineDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
            _forwardPipelineDescriptor.primitive.topology = subMesh->topology();

            auto renderPipeline = ResourceCache::GetSingleton().requestPipeline(_forwardPipelineDescriptor);
            passEncoder.SetPipeline(renderPipeline);
        }

        // Draw Call
        for (uint32_t j = 0; j < mesh->vertexBufferBindings().size(); j++) {
            auto vertexBufferBinding = mesh->vertexBufferBindings()[j];
            if (vertexBufferBinding) {
                passEncoder.SetVertexBuffer(j, mesh->vertexBufferBindings()[j]->handle());
            }
        }
        auto indexBufferBinding = mesh->indexBufferBinding();
        if (indexBufferBinding) {
            passEncoder.SetIndexBuffer(mesh->indexBufferBinding()->buffer(), mesh->indexBufferBinding()->format());
        }
        passEncoder.DrawIndexed(subMesh->count(), 1, subMesh->start(), 0, 0);
    }
}

// MARK: - Picker
Vector3F ColorPickerSubpass::id2Color(uint32_t id) {
    if (id >= 0xffffff) {
        std::cout << "Framebuffer Picker encounter primitive's id greater than " + std::to_string(0xffffff)
                  << std::endl;
        return {0.f, 0.f, 0.f};
    }

    return {(id & 0xff) / 255.f, ((id & 0xff00) >> 8) / 255.f, ((id & 0xff0000) >> 16) / 255.f};
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

}  // namespace vox
