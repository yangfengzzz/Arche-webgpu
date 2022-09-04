//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/enums/render_queue_type.h"
#include "vox.render/shader/shader_data.h"
#include "vox.render/shader/shader_source.h"
#include "vox.render/shader/state/render_state.h"

namespace vox {
/**
 * Material.
 */
class Material {
public:
    /** Name. */
    std::string name;

    /** Render queue type. */
    RenderQueueType::Enum renderQueueType = RenderQueueType::Enum::Opaque;

    /** Shader used by the material. */
    std::shared_ptr<ShaderSource> vertex_source_{nullptr};
    std::shared_ptr<ShaderSource> fragment_source_{nullptr};

    /** Shader data. */
    ShaderData shaderData;

    /** Render state. */
    RenderState renderState = RenderState();

    /**
     * Create a material instance.
     */
    explicit Material(wgpu::Device &device, std::string name = "");

protected:
    wgpu::Device &device_;
};
using MaterialPtr = std::shared_ptr<Material>;

}  // namespace vox