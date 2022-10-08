//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/render_primitive.h"

namespace vox::physics_debugger {
void RenderPrimitive::Clear() {
    ReleaseVertexBuffer();
    ReleaseIndexBuffer();
}

void RenderPrimitive::CreateVertexBuffer(int inNumVtx, int inVtxSize, const void* inData) {
    ReleaseVertexBuffer();

    wgpu::BufferDescriptor desc;
    desc.size = uint64(inNumVtx) * inVtxSize;
    desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    mVtxBuffer = device.CreateBuffer(&desc);
    device.GetQueue().WriteBuffer(mVtxBuffer, 0, inData, desc.size);

    mNumVtx = inNumVtx;
    mNumVtxToDraw = inNumVtx;
    mVtxSize = inVtxSize;
}

void RenderPrimitive::ReleaseVertexBuffer() {
    if (mVtxBuffer != nullptr) {
        mVtxBuffer.Release();
    }
    mNumVtx = 0;
    mNumVtxToDraw = 0;
    mVtxSize = 0;
}

void* RenderPrimitive::LockVertexBuffer() {
    mVtxBuffer.MapAsync(
            wgpu::MapMode::Write, 0, 4,
            [](WGPUBufferMapAsyncStatus status, void* userdata) {
                auto* app = static_cast<RenderPrimitive*>(userdata);
                if (status == WGPUBufferMapAsyncStatus_Success) {
                    app->vertex_mapped_resource = app->mVtxBuffer.GetMappedRange();
                } else {
                    app->vertex_mapped_resource = nullptr;
                }
            },
            this);

    while (vertex_mapped_resource == nullptr) {
    }
    return vertex_mapped_resource;
}

void RenderPrimitive::UnlockVertexBuffer() {
    mVtxBuffer.Unmap();
    vertex_mapped_resource = nullptr;
}

void RenderPrimitive::CreateIndexBuffer(int inNumIdx, const uint32_t* inData) {
    ReleaseIndexBuffer();

    wgpu::BufferDescriptor desc;
    desc.size = uint64(inNumIdx) * sizeof(uint32);
    desc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    mIdxBuffer = device.CreateBuffer(&desc);
    device.GetQueue().WriteBuffer(mIdxBuffer, 0, inData, desc.size);

    mNumIdx = inNumIdx;
    mNumIdxToDraw = inNumIdx;
}

void RenderPrimitive::ReleaseIndexBuffer() {
    if (mIdxBuffer != nullptr) {
        mIdxBuffer.Release();
    }
    mNumIdx = 0;
    mNumIdxToDraw = 0;
}

uint32* RenderPrimitive::LockIndexBuffer() {
    mIdxBuffer.MapAsync(
            wgpu::MapMode::Write, 0, 4,
            [](WGPUBufferMapAsyncStatus status, void* userdata) {
                auto* app = static_cast<RenderPrimitive*>(userdata);
                if (status == WGPUBufferMapAsyncStatus_Success) {
                    app->index_mapped_resource = static_cast<uint32_t*>(app->mVtxBuffer.GetMappedRange());
                } else {
                    app->index_mapped_resource = nullptr;
                }
            },
            this);

    while (index_mapped_resource == nullptr) {
    }
    return index_mapped_resource;
}

void RenderPrimitive::UnlockIndexBuffer() {
    mIdxBuffer.Unmap();
    index_mapped_resource = nullptr;
}

void RenderPrimitive::Draw(wgpu::RenderPassEncoder& passEncoder) const {
    // Draw Call
    passEncoder.SetVertexBuffer(0, mVtxBuffer);

    if (mIdxBuffer != nullptr) {
        passEncoder.SetIndexBuffer(mIdxBuffer, wgpu::IndexFormat::Uint32);
        passEncoder.DrawIndexed(mNumIdxToDraw);
    } else {
        passEncoder.Draw(mNumVtxToDraw);
    }
}

}  // namespace vox::physics_debugger