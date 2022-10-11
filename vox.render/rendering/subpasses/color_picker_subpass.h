//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/buffer_pool.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox {
class ColorPickerSubpass : public GeometrySubpass {
public:
    ColorPickerSubpass(RenderContext* renderContext,
                       wgpu::TextureFormat depthStencilTextureFormat,
                       Scene* scene,
                       Camera* camera);

    void _drawElement(wgpu::RenderPassEncoder& passEncoder, ShaderVariant& variant) override;

public:
    void addExclusiveRenderer(Renderer *renderer);

    void clearExclusiveList();

public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    static Vector3F id2Color(uint32_t id);

    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    static uint32_t color2Id(const std::array<uint8_t, 4>& color);

    /**
     * Get renderer element by color.
     */
    std::pair<Renderer*, MeshPtr> getObjectByColor(const std::array<uint8_t, 4>& color);

private:
    void _uploadColor(RenderElement& element);

    uint32_t _currentId = 0;
    std::unordered_map<size_t, std::pair<Renderer*, MeshPtr>> _primitivesMap{};
    BufferPool _bufferPool;
    std::shared_ptr<UnlitMaterial> _material{nullptr};

    std::vector<Renderer *> _exclusive_list{};
};

}  // namespace vox