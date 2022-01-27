//
//  index_buffer_binding.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

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
    wgpu::Buffer& buffer();

    /**
     * Index buffer format.
     */
    wgpu::IndexFormat format();
    
private:
    Buffer _buffer;
    wgpu::IndexFormat _format;
};

}

#endif /* index_buffer_binding_hpp */
