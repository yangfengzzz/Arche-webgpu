//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/render_primitive.h"

namespace vox::physics_debugger {
void RenderPrimitive::Clear() {}

void RenderPrimitive::CreateVertexBuffer(int inNumVtx, int inVtxSize, const void *inData) {}

void RenderPrimitive::UpdateVertexBuffer() {}

void RenderPrimitive::CreateIndexBuffer(int inNumIdx, const uint32_t *inData) {}

void RenderPrimitive::UpdateIndexBuffer() {}

}  // namespace vox::physics_debugger