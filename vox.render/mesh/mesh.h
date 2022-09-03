//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_hpp
#define mesh_hpp

#include <optional>
#include <string>

#include "index_buffer_binding.h"
#include "sub_mesh.h"
#include "update_flag_manager.h"
#include "vox.math/bounding_box3.h"

namespace vox {
class Mesh {
public:
    /** Name. */
    std::string name = "";
    /** The bounding volume of the mesh. */
    BoundingBox3F bounds = BoundingBox3F();

    /**
     * Instanced count, disable instanced drawing when set zero.
     */
    uint32_t instanceCount() const;

    /**
     * First sub-mesh. Rendered using the first material.
     */
    const SubMesh* subMesh() const;

    SubMesh* subMesh();

    /**
     * A collection of sub-mesh, each sub-mesh can be rendered with an independent material.
     */
    const std::vector<SubMesh>& subMeshes() const;

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param subMesh - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set, it means the offset in the vertex buffer
     */
    void addSubMesh(SubMesh subMesh);

    /**
     * Add sub-mesh, each sub-mesh can correspond to an independent material.
     * @param start - Start drawing offset, if the index buffer is set,
     * it means the offset in the index buffer, if not set,
     * it means the offset in the vertex buffer
     * @param count - Drawing count, if the index buffer is set,
     * it means the count in the index buffer, if not set,
     * it means the count in the vertex buffer
     * @param topology - Drawing topology, default is MeshTopology.Triangles
     */
    void addSubMesh(uint32_t start = 0,
                    uint32_t count = 0,
                    wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList);

    /**
     * Clear all sub-mesh.
     */
    void clearSubMesh();

    /**
     * Register update flag, update flag will be true if the vertex element changes.
     * @returns Update flag
     */
    std::unique_ptr<UpdateFlag> registerUpdateFlag();

public:
    const std::vector<wgpu::VertexBufferLayout>& vertexBufferLayouts() const;

    const std::vector<std::optional<Buffer>>& vertexBufferBindings() const;

    const std::optional<IndexBufferBinding>& indexBufferBinding() const;

protected:
    void _setVertexLayouts(const std::vector<wgpu::VertexBufferLayout>& layouts);

    void _setVertexBufferBinding(size_t index, const Buffer& binding);

    void _setIndexBufferBinding(std::optional<IndexBufferBinding> binding);

protected:
    uint32_t _instanceCount = 1;
    std::vector<std::optional<Buffer>> _vertexBufferBindings{};
    std::optional<IndexBufferBinding> _indexBufferBinding = std::nullopt;
    std::vector<wgpu::VertexBufferLayout> _vertexBufferLayouts{};

    std::vector<SubMesh> _subMeshes{};
    UpdateFlagManager _updateFlagManager;

private:
    void _clearVertexLayouts();

    void _addVertexLayout(const wgpu::VertexBufferLayout& layout);
};

}  // namespace vox

#endif /* mesh_hpp */
