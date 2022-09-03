//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef index_buffer_binding_hpp
#define index_buffer_binding_hpp

#include "buffer.h"

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
    const wgpu::Buffer& buffer() const;

    /**
     * Index buffer format.
     */
    wgpu::IndexFormat format() const;

private:
    Buffer _buffer;
    wgpu::IndexFormat _format;
};

}  // namespace vox

#endif /* index_buffer_binding_hpp */
