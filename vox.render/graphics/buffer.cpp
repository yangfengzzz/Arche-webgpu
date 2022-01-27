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

Buffer::Buffer(wgpu::Device& device, std::vector<uint8_t>& data, wgpu::BufferUsage bufferUsage) {
    wgpu::BufferDescriptor desc;
    desc.usage = bufferUsage;
    desc.size = data.size();
    _nativeBuffer = device.CreateBuffer(&desc);
    
    setData(data, 0, data.size());
}

const wgpu::Buffer& Buffer::buffer() const {
    return _nativeBuffer;
}

void Buffer::setData(std::vector<uint8_t>& data,
                     size_t bufferByteOffset,
                     size_t dataLength,
                     size_t dataOffset) {
    struct UserData {
        Buffer* dst_ptr;
        uint8_t* src_ptr;
        size_t length;
        size_t bufferByteOffset;
        size_t dataLength;
        size_t dataOffset;
    };
    UserData user{this, data.data(), data.size(), bufferByteOffset, dataLength, dataOffset};
    
    _nativeBuffer.MapAsync(wgpu::MapMode::Write, 0, data.size(), [](WGPUBufferMapAsyncStatus status, void * userdata) {
        if (static_cast<wgpu::BufferMapAsyncStatus>(status) == wgpu::BufferMapAsyncStatus::Success) {
            UserData* user = static_cast<UserData*>(userdata);
            memcpy(user->dst_ptr->_nativeBuffer.GetMappedRange(user->dataOffset, user->dataLength),
                   user->src_ptr + user->bufferByteOffset, user->length);
            user->dst_ptr->_nativeBuffer.Unmap();
        }
    }, &user);
}

void Buffer::getData(std::vector<uint8_t>& data,
                     size_t bufferByteOffset,
                     size_t dataLength,
                     size_t dataOffset) {
    struct UserData {
        Buffer* dst_ptr;
        uint8_t* src_ptr;
        size_t length;
        size_t bufferByteOffset;
        size_t dataLength;
        size_t dataOffset;
    };
    UserData user{this, data.data(), data.size(), bufferByteOffset, dataLength, dataOffset};
    
    _nativeBuffer.MapAsync(wgpu::MapMode::Read, 0, data.size(), [](WGPUBufferMapAsyncStatus status, void * userdata) {
        if (static_cast<wgpu::BufferMapAsyncStatus>(status) == wgpu::BufferMapAsyncStatus::Success) {
            UserData* user = static_cast<UserData*>(userdata);
            memcpy(user->src_ptr + user->bufferByteOffset,
                   user->dst_ptr->_nativeBuffer.GetMappedRange(user->dataOffset, user->dataLength),
                   user->length);
            user->dst_ptr->_nativeBuffer.Unmap();
        }
    }, &user);
}

}
