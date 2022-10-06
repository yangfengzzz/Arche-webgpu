//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Core/Reference.h>
#include <webgpu/webgpu_cpp.h>

using namespace JPH;

namespace vox::physics_debugger {
class RenderPrimitive : public RefTarget<RenderPrimitive> {
public:
    RenderPrimitive(wgpu::Device &device, wgpu::PrimitiveTopology inType) : mDevice(device), mType(inType) {}

    ~RenderPrimitive() { Clear(); }

    void Clear();

    [[nodiscard]] bool IsEmpty() const { return mNumVtx == 0 && mNumIdx == 0; }

public:
    /// Vertex buffer management functions
    void CreateVertexBuffer(int inNumVtx, int inVtxSize, const void *inData = nullptr);

    void ReleaseVertexBuffer();

    void *LockVertexBuffer();

    void UnlockVertexBuffer();

    [[nodiscard]] int GetNumVtx() const { return mNumVtx; }

    [[nodiscard]] int GetNumVtxToDraw() const { return mNumVtxToDraw; }

    void SetNumVtxToDraw(int inUsed) { mNumVtxToDraw = inUsed; }

public:
    /// Index buffer management functions
    void CreateIndexBuffer(int inNumIdx, const uint32_t *inData = nullptr);

    void ReleaseIndexBuffer();

    uint32_t *LockIndexBuffer();

    void UnlockIndexBuffer();

    [[nodiscard]] int GetNumIdx() const { return mNumIdx; }

    [[nodiscard]] int GetNumIdxToDraw() const { return mNumIdxToDraw; }

    void SetNumIdxToDraw(int inUsed) { mNumIdxToDraw = inUsed; }

public:
    /// Draw the primitive
    void Draw() const;

private:
    friend class RenderInstances;

    wgpu::Device &mDevice;

    wgpu::PrimitiveTopology mType;

    wgpu::Buffer mVtxBuffer;
    int mNumVtx = 0;
    int mNumVtxToDraw = 0;
    int mVtxSize = 0;
    bool mVtxBufferInUploadHeap = false;

    wgpu::Buffer mIdxBuffer;
    int mNumIdx = 0;
    int mNumIdxToDraw = 0;
    bool mIdxBufferInUploadHeap = false;
};
}  // namespace vox::physics_debugger