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

namespace vox {
ColorPickerSubpass::ColorPickerSubpass(RenderContext *renderContext,
                                       wgpu::TextureFormat depthStencilTextureFormat,
                                       Scene *scene,
                                       Camera *camera)
    : GeometrySubpass(renderContext, depthStencilTextureFormat, scene, camera),
      _bufferPool(renderContext->device(), 16 * 1024, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst) {
    _material = std::make_shared<UnlitMaterial>(renderContext->device());
}

void ColorPickerSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder, ShaderVariant &variant) {
    _currentId = 0;
    _primitivesMap.clear();

    _callRender(_camera);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    for (auto &element : opaqueQueue) {
        auto exclusive = std::find(_exclusive_list.begin(), _exclusive_list.end(), element.renderer);
        if (exclusive != _exclusive_list.end()) {
            continue;
        }

        _uploadColor(element);
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (auto &element : alphaTestQueue) {
        auto exclusive = std::find(_exclusive_list.begin(), _exclusive_list.end(), element.renderer);
        if (exclusive != _exclusive_list.end()) {
            continue;
        }

        _uploadColor(element);
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (auto &element : transparentQueue) {
        auto exclusive = std::find(_exclusive_list.begin(), _exclusive_list.end(), element.renderer);
        if (exclusive != _exclusive_list.end()) {
            continue;
        }

        _uploadColor(element);
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
}

void ColorPickerSubpass::_uploadColor(RenderElement &element) {
    element.material = _material;
    _primitivesMap[_currentId] = std::make_pair(element.renderer, element.mesh);
    Vector3F color = id2Color(_currentId);
    auto reverseColor = Color(color.z, color.y, color.x, 1);

    auto &bufferBlock = _bufferPool.requestBufferBlock(sizeof(Color));
    auto allocation = bufferBlock.allocate(sizeof(Color));
    allocation.update(_renderContext->device(), reverseColor);
    _material->shaderData.setData("u_baseColor", std::move(allocation));

    _currentId += 1;
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

void ColorPickerSubpass::addExclusiveRenderer(Renderer *renderer) { _exclusive_list.emplace_back(renderer); }

void ColorPickerSubpass::clearExclusiveList() { _exclusive_list.clear(); }

}  // namespace vox
