//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/buffer.h"

namespace vox {
Buffer::Buffer(wgpu::Device& device, uint64_t byteLength, wgpu::BufferUsage bufferUsage) : _size(byteLength) {
    wgpu::BufferDescriptor desc;
    desc.usage = bufferUsage;
    desc.size = byteLength;
    _nativeBuffer = device.CreateBuffer(&desc);
}

Buffer::Buffer(wgpu::Device& device, const void* data, uint64_t size, wgpu::BufferUsage bufferUsage) {
    wgpu::BufferDescriptor desc;
    desc.usage = bufferUsage;
    desc.size = size;
    _nativeBuffer = device.CreateBuffer(&desc);

    uploadData(device, data, size);
}

Buffer::~Buffer() { _nativeBuffer.Release(); }

void Buffer::uploadData(wgpu::Device& device, const void* data, uint64_t size, uint64_t bufferOffset) {
    device.GetQueue().WriteBuffer(_nativeBuffer, bufferOffset, data, size);
}

const wgpu::Buffer& Buffer::handle() const { return _nativeBuffer; }

uint64_t Buffer::size() const { return _size; }

}  // namespace vox
