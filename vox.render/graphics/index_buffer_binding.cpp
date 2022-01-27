//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "index_buffer_binding.h"

namespace vox {
IndexBufferBinding::IndexBufferBinding(Buffer buffer, wgpu::IndexFormat format):
_buffer(buffer),
_format(format) {
}

wgpu::Buffer& IndexBufferBinding::buffer() {
    return _buffer.buffer();
}

wgpu::IndexFormat IndexBufferBinding::format() {
    return _format;
    
}

}
