//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.toolkit/physics_debugger/render_primitive.h"

namespace vox::physics_debugger {

/// Buffer that holds a list of instances (usually model transform etc.) for instance based rendering
class RenderInstances : public RefTarget<RenderInstances> {
public:
    /// Constructor
    explicit RenderInstances(wgpu::Device& device) : device(device) {}
    ~RenderInstances() { Clear(); }

    /// Erase all instance data
    void Clear();

    /// Instance buffer management functions
    void CreateBuffer(int inNumInstances, int inInstanceSize);
    void* Lock();
    void Unlock();

    /// Draw the instances when context has been set by Renderer::BindShader
    void Draw(wgpu::RenderPassEncoder& passEncoder,
              RenderPrimitive* inPrimitive,
              int inStartInstance,
              int inNumInstances) const;

private:
    wgpu::Device& device;

    void* mapped_ptr{nullptr};
    wgpu::Buffer mInstanceBuffer;
    int mInstanceBufferSize = 0;
    int mInstanceSize = 0;
};
}  // namespace vox::physics_debugger