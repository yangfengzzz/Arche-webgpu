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

void RenderPrimitive::CreateVertexBuffer(int inNumVtx, int inVtxSize, const void *inData) {
    mVtxBuffer.Release();
    mNumVtx = 0;
    mNumVtxToDraw = 0;
    mVtxSize = 0;
    mVtxBufferInUploadHeap = false;
}
void RenderPrimitive::ReleaseVertexBuffer() {}
void *RenderPrimitive::LockVertexBuffer() {}
void RenderPrimitive::UnlockVertexBuffer() {
    mVtxBuffer.Unmap();
}

void RenderPrimitive::CreateIndexBuffer(int inNumIdx, const uint32_t *inData) {}
void RenderPrimitive::ReleaseIndexBuffer() {
    mIdxBuffer.Release();
    mNumIdx = 0;
    mNumIdxToDraw = 0;
    mIdxBufferInUploadHeap = false;
}
uint32_t *RenderPrimitive::LockIndexBuffer() {}
void RenderPrimitive::UnlockIndexBuffer() {
    mIdxBuffer.Unmap();
}

void RenderPrimitive::Draw() const {}

}  // namespace vox::physics_debugger