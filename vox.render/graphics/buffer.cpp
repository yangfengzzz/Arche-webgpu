//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "buffer.h"

namespace vox {
Buffer::Buffer(wgpu::Device& device, uint64_t byteLength, wgpu::BufferUsage bufferUsage) {
    wgpu::BufferDescriptor desc;
    desc.usage = bufferUsage;
    desc.size = byteLength;
    _nativeBuffer = device.CreateBuffer(&desc);
}

Buffer::Buffer(wgpu::Device& device, const void* data,
               uint64_t size, wgpu::BufferUsage bufferUsage) {
    wgpu::BufferDescriptor desc;
    desc.usage = bufferUsage;
    desc.size = size;
    _nativeBuffer = device.CreateBuffer(&desc);
    
    device.GetQueue().WriteBuffer(_nativeBuffer, 0, data, size);
}

const wgpu::Buffer& Buffer::buffer() const {
    return _nativeBuffer;
}

}
