//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/render_instances.h"

namespace vox::physics_debugger {
void RenderInstances::Clear() {
    mInstanceBuffer.Release();
    mInstanceBuffer = nullptr;
    mInstanceBufferSize = 0;
    mInstanceSize = 0;
}

void RenderInstances::CreateBuffer(int inNumInstances, int inInstanceSize) {
    if (mInstanceBuffer == nullptr || mInstanceBufferSize < inNumInstances * inInstanceSize) {
        // Delete the old buffer
        Clear();

        // Calculate size
        mInstanceBufferSize = inNumInstances * inInstanceSize;

        wgpu::BufferDescriptor desc;
        desc.size = mInstanceBufferSize;
        desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
        mInstanceBuffer = device.CreateBuffer(&desc);
    }

    // Update parameters
    mInstanceSize = inInstanceSize;
}

void* RenderInstances::Lock() {
    mInstanceBuffer.MapAsync(
            wgpu::MapMode::Write, 0, 4,
            [](WGPUBufferMapAsyncStatus status, void* userdata) {
                auto* app = static_cast<RenderInstances*>(userdata);
                if (status == WGPUBufferMapAsyncStatus_Success) {
                    app->mapped_ptr = app->mInstanceBuffer.GetMappedRange();
                } else {
                    app->mapped_ptr = nullptr;
                }
            },
            this);

    while (mapped_ptr == nullptr) {
    }
    return mapped_ptr;
}
void RenderInstances::Unlock() {
    mInstanceBuffer.Unmap();
    mapped_ptr = nullptr;
}

void RenderInstances::Draw(wgpu::RenderPassEncoder& passEncoder,
                           RenderPrimitive* inPrimitive,
                           int inStartInstance,
                           int inNumInstances) const {
    // Draw Call
    passEncoder.SetVertexBuffer(0, inPrimitive->mVtxBuffer);
    passEncoder.SetVertexBuffer(1, mInstanceBuffer);

    if (inPrimitive->mIdxBuffer != nullptr) {
        passEncoder.SetIndexBuffer(inPrimitive->mIdxBuffer, wgpu::IndexFormat::Uint32);
        passEncoder.DrawIndexed(inPrimitive->mNumIdxToDraw, inNumInstances, 0, 0, inStartInstance);
    } else {
        passEncoder.Draw(inPrimitive->mNumIdxToDraw, inNumInstances, 0, inStartInstance);
    }
}

}  // namespace vox::physics_debugger