//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.navigation/nav_debug_render.h"

namespace vox::nav {
void NavDebugRender::depthMask(bool state) {}

void NavDebugRender::texture(bool state) {}

void NavDebugRender::begin(duDebugDrawPrimitives prim, float size) {}

void NavDebugRender::vertex(const float* pos, unsigned int color) {}

void NavDebugRender::vertex(const float x, const float y, const float z, unsigned int color) {}

void NavDebugRender::vertex(const float* pos, unsigned int color, const float* uv) {}

void NavDebugRender::vertex(
        const float x, const float y, const float z, unsigned int color, const float u, const float v) {}

void NavDebugRender::end() {}

}  // namespace vox::nav