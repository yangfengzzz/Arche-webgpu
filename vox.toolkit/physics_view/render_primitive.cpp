//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_view/render_primitive.h"

namespace vox::physics_view {
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
    if (inData) {
        device.GetQueue().WriteBuffer(mVtxBuffer, 0, inData, desc.size);
    }

    mNumVtx = inNumVtx;
    mNumVtxToDraw = inNumVtx;
    mVtxSize = inVtxSize;
    vertex_mapped_resource.resize(desc.size / sizeof(float));
}

void RenderPrimitive::ReleaseVertexBuffer() {
    if (mVtxBuffer != nullptr) {
        mVtxBuffer.Release();
    }
    mNumVtx = 0;
    mNumVtxToDraw = 0;
    mVtxSize = 0;
    vertex_mapped_resource.clear();
}

void* RenderPrimitive::LockVertexBuffer() { return vertex_mapped_resource.data(); }

void RenderPrimitive::UnlockVertexBuffer() {
    device.GetQueue().WriteBuffer(mVtxBuffer, 0, vertex_mapped_resource.data(),
                                  vertex_mapped_resource.size() * sizeof(float));
}

void RenderPrimitive::CreateIndexBuffer(int inNumIdx, const uint32_t* inData) {
    ReleaseIndexBuffer();

    wgpu::BufferDescriptor desc;
    desc.size = uint64(inNumIdx) * sizeof(uint32);
    desc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
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

}  // namespace vox::physics_view