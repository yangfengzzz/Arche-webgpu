//
//  buffer.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

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
    
    wgpu::Buffer& buffer();
    
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
    wgpu::Device& _device;
    wgpu::Buffer _nativeBuffer;
};

}

#endif /* buffer_hpp */
