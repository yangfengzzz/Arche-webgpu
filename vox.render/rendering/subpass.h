//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/render_element.h"
#include "vox.render/scene.h"

namespace vox {
class RenderPass;

class Subpass {
public:
    Subpass(RenderContext *renderContext, Scene *scene, Camera *camera);

    Subpass(const Subpass &) = delete;

    Subpass(Subpass &&) = default;

    virtual ~Subpass() = default;

    Subpass &operator=(const Subpass &) = delete;

    Subpass &operator=(Subpass &&) = delete;

    void setRenderPass(RenderPass *pass);

    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(wgpu::RenderPassEncoder &commandEncoder) = 0;

    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void prepare() = 0;

protected:
    static bool _compareFromNearToFar(const RenderElement &a, const RenderElement &b);

    static bool _compareFromFarToNear(const RenderElement &a, const RenderElement &b);

    RenderPass *_pass{nullptr};

    RenderContext *_renderContext{nullptr};
    Scene *_scene{nullptr};
    Camera *_camera{nullptr};
};

}  // namespace vox