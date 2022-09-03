//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

namespace vox {
/**
 * Depth state.
 */
struct DepthState {
    /** Whether to enable the depth test. */
    bool enabled = true;
    /** Whether the depth value can be written.*/
    bool writeEnabled = true;
    /** Depth comparison function. */
    wgpu::CompareFunction compareFunction = wgpu::CompareFunction::Less;

    void apply(wgpu::DepthStencilState *depthStencil) const { platformApply(depthStencil); }

    void platformApply(wgpu::DepthStencilState *depthStencil) const;
};

}  // namespace vox