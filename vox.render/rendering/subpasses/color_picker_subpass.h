//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef color_picker_subpass_hpp
#define color_picker_subpass_hpp

#include "rendering/subpass.h"
#include "material/unlit_material.h"

namespace vox {
class ColorPickerSubpass: public Subpass {
public:
    ColorPickerSubpass(RenderContext* renderContext,
                       Scene* scene,
                       Camera* camera);
    
    void prepare() override;
    
    void draw(wgpu::RenderPassEncoder& passEncoder) override;
    
public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    Vector3F id2Color(uint32_t id);
    
    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    uint32_t color2Id(const std::array<uint8_t, 4> &color);
    
    /**
     * Get renderer element by color.
     */
    std::pair<Renderer *, MeshPtr> getObjectByColor(const std::array<uint8_t, 4> &color);
    
private:    
    void _drawMeshes(wgpu::RenderPassEncoder &passEncoder);
    
    void _drawElement(wgpu::RenderPassEncoder &passEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    void _bindingData(wgpu::BindGroupEntry& entry,
                      Buffer& buffer, Renderer* renderer);
    
    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::DepthStencilState _depthStencil;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;
    
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    
    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
    
    uint32_t _currentId = 0;
    std::unordered_map<size_t, std::pair<Renderer *, MeshPtr>> _primitivesMap;
    std::vector<Buffer> _bufferPool;
    std::shared_ptr<UnlitMaterial> _material{nullptr};
};

}

#endif /* color_picker_subpass_hpp */
