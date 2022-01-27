//
//  index_buffer_binding.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

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
