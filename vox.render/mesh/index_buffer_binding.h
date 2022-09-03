//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/mesh/buffer.h"

namespace vox {
/**
 * Index buffer binding.
 */
class IndexBufferBinding {
public:
    /**
     * Create index buffer binding.
     * @param buffer - Index buffer
     * @param format - Index buffer format
     */
    IndexBufferBinding(Buffer buffer, wgpu::IndexFormat format);

    /**
     * Index buffer.
     */
    [[nodiscard]] const wgpu::Buffer& buffer() const;

    /**
     * Index buffer format.
     */
    [[nodiscard]] wgpu::IndexFormat format() const;

private:
    Buffer _buffer;
    wgpu::IndexFormat _format;
};

}  // namespace vox