//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.toolkit/debug_draw/debug_draw.h"

namespace vox::debug {
class DebugDrawSubpass : public DebugDraw {
public:
    void depthMask(bool state) override;
    void texture(bool state) override;
    void begin(wgpu::PrimitiveTopology prim, float size = 1.0f) override;
    void vertex(const float* pos, unsigned int color) override;
    void vertex(float x, float y, float z, unsigned int color) override;
    void vertex(const float* pos, unsigned int color, const float* uv) override;
    void vertex(float x, float y, float z, unsigned int color, float u, float v) override;
    void end() override;
};
}  // namespace vox::debug