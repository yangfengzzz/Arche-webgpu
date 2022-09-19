//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpasses/forward_subpass.h"

namespace vox {
class GeometrySubpass : public ForwardSubpass {
public:
    GeometrySubpass(RenderContext* renderContext,
                    wgpu::TextureFormat depthStencilTextureFormat,
                    Scene* scene,
                    Camera* camera);

    void _drawElement(wgpu::RenderPassEncoder& passEncoder, ShaderVariant& variant) override;

protected:
    void _callRender(Camera* camera);

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
};

}  // namespace vox