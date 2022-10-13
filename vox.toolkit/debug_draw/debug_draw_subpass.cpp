//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/debug_draw/debug_draw_subpass.h"

namespace vox::debug {
void DebugDrawSubpass::depthMask(bool state) {}

void DebugDrawSubpass::texture(bool state) {}

void DebugDrawSubpass::begin(wgpu::PrimitiveTopology prim, float size) {}

void DebugDrawSubpass::vertex(const float* pos, unsigned int color) {}

void DebugDrawSubpass::vertex(const float x, const float y, const float z, unsigned int color) {}

void DebugDrawSubpass::vertex(const float* pos, unsigned int color, const float* uv) {}

void DebugDrawSubpass::vertex(
        const float x, const float y, const float z, unsigned int color, const float u, const float v) {}

void DebugDrawSubpass::end() {}

}  // namespace vox::debug