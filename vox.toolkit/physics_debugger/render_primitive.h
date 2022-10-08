//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Core/Reference.h>

#include <utility>

#include "vox.render/mesh/buffer_mesh.h"

using namespace JPH;

namespace vox::physics_debugger {
/// Simple wrapper around vertex and index buffers
class RenderPrimitive : public RefTarget<RenderPrimitive> {
public:
    /// Constructor
    RenderPrimitive(wgpu::Device& device, wgpu::PrimitiveTopology inType) : device(device), mType(inType) {}
    ~RenderPrimitive() { Clear(); }

    /// Erase all primitive data
    void Clear();

    /// Check if this primitive contains any data
    bool IsEmpty() const { return mNumVtx == 0 && mNumIdx == 0; }

    /// Vertex buffer management functions
    void CreateVertexBuffer(int inNumVtx, int inVtxSize, const void* inData = nullptr);
    void UpdateVertexBuffer();
    int GetNumVtx() const { return mNumVtx; }
    int GetNumVtxToDraw() const { return mNumVtxToDraw; }
    void SetNumVtxToDraw(int inUsed) { mNumVtxToDraw = inUsed; }

    /// Index buffer management functions
    void CreateIndexBuffer(int inNumIdx, const uint32_t* inData = nullptr);
    void UpdateIndexBuffer();
    int GetNumIdx() const { return mNumIdx; }
    int GetNumIdxToDraw() const { return mNumIdxToDraw; }
    void SetNumIdxToDraw(int inUsed) { mNumIdxToDraw = inUsed; }

    /// Draw the primitive
    void Draw(wgpu::RenderPassEncoder& passEncoder) const;

private:
    friend class RenderInstances;

    wgpu::Device& device;

    wgpu::PrimitiveTopology mType;

    std::unique_ptr<Buffer> mVtxBuffer;
    int mNumVtx = 0;
    int mNumVtxToDraw = 0;
    int mVtxSize = 0;
    bool mVtxBufferInUploadHeap = false;

    std::unique_ptr<Buffer> mIdxBuffer;
    int mNumIdx = 0;
    int mNumIdxToDraw = 0;
    bool mIdxBufferInUploadHeap = false;
};
}  // namespace vox::physics_debugger