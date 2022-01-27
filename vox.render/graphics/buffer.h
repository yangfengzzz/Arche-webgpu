//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef buffer_hpp
#define buffer_hpp

#include <dawn/webgpu_cpp.h>
#include <vector>

namespace vox {
class Buffer {
public:
    /**
     * Create Buffer.
     * @param device - Device
     * @param byteLength - Byte length
     * @param bufferUsage - Buffer usage
     */
    Buffer(wgpu::Device& device, uint64_t byteLength, wgpu::BufferUsage bufferUsage);
    
    /**
     * Create Buffer.
     * @param device - Device
     * @param data - Byte
     * @param bufferUsage - Buffer usage
     */
    Buffer(wgpu::Device& device, std::vector<uint8_t>& data, wgpu::BufferUsage bufferUsage);
    
    Buffer(Buffer const& other) = default;
    
    Buffer(Buffer&& other) = default;
    
    Buffer& operator=(Buffer const& other) = default;
    
    Buffer& operator=(Buffer&& other) = default;
    
public:
    const wgpu::Buffer& buffer() const;
    
    /**
     * Set buffer data.
     * @param data - Input buffer data
     * @param bufferByteOffset - Buffer byte offset
     * @param dataLength - Data length
     * @param dataOffset - Buffer byte offset
     */
    void setData(std::vector<uint8_t>& data, size_t bufferByteOffset = 0,
                 size_t dataLength = 0, size_t dataOffset = 0);
    
    /**
     * Set buffer data.
     * @param data - Input buffer data
     * @param bufferByteOffset - Buffer byte offset
     * @param dataLength - Data length
     * @param dataOffset - Buffer byte offset
     */
    void getData(std::vector<uint8_t>& data, size_t bufferByteOffset = 0,
                 size_t dataLength = 0, size_t dataOffset = 0);
    
private:
    wgpu::Buffer _nativeBuffer;
};

}

#endif /* buffer_hpp */
