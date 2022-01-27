//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef buffer_mesh_hpp
#define buffer_mesh_hpp

#include "graphics/mesh.h"

namespace vox {
class BufferMesh:public Mesh {
public:
    /**
     * Instanced count, disable instanced drawing when set zero.
     */
    size_t instanceCount();
    
    void setInstanceCount(size_t newValue);
    
    /**
     * Vertex buffer binding collection.
     */
    const std::vector<std::optional<Buffer>>& vertexBufferBindings();

    /**
     * Index buffer binding.
     */
    const std::optional<IndexBufferBinding>& indexBufferBinding();
    
    /**
     * Vertex layout collection.
     */
    const std::vector<wgpu::VertexBufferLayout>& vertexBufferLayouts();

    /**
     * Set vertex layouts.
     * @param layouts - Vertex layouts collection
     */
    void setVertexLayouts(const std::vector<wgpu::VertexBufferLayout>& layouts);
    
    /**
     * Set vertex buffer binding.
     * @param buffer - Vertex buffer binding
     * @param index - Vertex buffer index, the default value is 0
     */
    void setVertexBufferBinding(const Buffer& buffer, size_t index = 0);

    /**
     * Set vertex buffer binding.
     * @param vertexBufferBindings - Vertex buffer binding
     * @param firstIndex - First vertex buffer index, the default value is 0
     */
    void setVertexBufferBindings(const std::vector<Buffer>& vertexBufferBindings, size_t firstIndex = 0);
    
    /**
     * Set index buffer binding.
     * @param buffer - Index buffer
     * @param format - Index buffer format
     */
    void setIndexBufferBinding(const Buffer& buffer, wgpu::IndexFormat format);

    /**
     * Set index buffer binding.
     * @param bufferBinding - Index buffer binding
     * @remarks When bufferBinding is null, it will clear IndexBufferBinding
     */
    void setIndexBufferBinding(std::optional<IndexBufferBinding> bufferBinding);
};

}

#endif /* buffer_mesh_hpp */
