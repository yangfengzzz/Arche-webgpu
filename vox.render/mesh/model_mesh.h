//
//  model_mesh.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef model_mesh_hpp
#define model_mesh_hpp

#include "graphics/mesh.h"
#include "color.h"
#include "texture/sampled_texture2d.h"

namespace vox {
struct ValueChanged {
    enum Enum {
        Position = 0x1,
        Normal = 0x2,
        Color = 0x4,
        Tangent = 0x8,
        BoneWeight = 0x10,
        BoneIndex = 0x20,
        UV = 0x40,
        UV1 = 0x80,
        UV2 = 0x100,
        UV3 = 0x200,
        UV4 = 0x400,
        UV5 = 0x800,
        UV6 = 0x1000,
        UV7 = 0x2000,
        BlendShape = 0x4000,
        All = 0xffff
    };
};

/**
 * Mesh containing common vertex elements of the model.
 */
class ModelMesh: public Mesh {
public:
    /**
     * Whether to access data of the mesh.
     */
    bool accessible();
    
    /**
     * Vertex count of current mesh.
     */
    size_t vertexCount();
    
    /**
     * Create a model mesh.
     * @param device - Engine to which the mesh belongs
     */
    ModelMesh(wgpu::Device &device);
    
public:
    /**
     * Set positions for the mesh.
     * @param positions - The positions for the mesh.
     */
    void setPositions(const std::vector<Vector3F> &positions);
    
    /**
     * Get positions for the mesh.
     * @remarks Please call the setPositions() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector3F> &positions();
    
    /**
     * Set per-vertex normals for the mesh.
     * @param normals - The normals for the mesh.
     */
    void setNormals(const std::vector<Vector3F> &normals);
    
    /**
     * Get normals for the mesh.
     * @remarks Please call the setNormals() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector3F> &normals();
    
    /**
     * Set per-vertex colors for the mesh.
     * @param colors - The colors for the mesh.
     */
    void setColors(const std::vector<Color> &colors);
    
    /**
     * Get colors for the mesh.
     * @remarks Please call the setColors() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Color> &colors();
    
    /**
     * Set per-vertex tangents for the mesh.
     * @param tangents - The tangents for the mesh.
     */
    void setTangents(const std::vector<Vector4F> &tangents);
    
    /**
     * Get tangents for the mesh.
     * @remarks Please call the setTangents() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector4F> &tangents();
    
    /**
     * Set per-vertex uv for the mesh by channelIndex.
     * @param uv - The uv for the mesh.
     * @param channelIndex - The index of uv channels, in [0 ~ 7] range.
     */
    void setUVs(const std::vector<Vector2F> &uv, int channelIndex = 0);
    
    /**
     * Get uv for the mesh by channelIndex.
     * @param channelIndex - The index of uv channels, in [0 ~ 7] range.
     * @remarks Please call the setUV() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector2F> &uvs(int channelIndex = 0);
    
    /**
     * Set indices for the mesh.
     * @param indices - The indices for the mesh.
     */
    void setIndices(const std::vector<uint32_t> &indices);
    
    /**
     * Set indices for the mesh.
     * @param indices - The indices for the mesh.
     */
    void setIndices(const std::vector<uint16_t> &indices);

    
    /**
     * Upload Mesh Data to the graphics API.
     * @param noLongerAccessible - Whether to access data later. If true, you'll never access data anymore (free memory cache)
     */
    void uploadData(bool noLongerAccessible);
    
private:
    wgpu::Device& _device;

    wgpu::VertexBufferLayout _updateVertexLayouts();
    
    void _updateVertices(std::vector<float> &vertices);
    
    void _releaseCache();
    
    bool _hasBlendShape = false;
    bool _useBlendShapeNormal = false;
    bool _useBlendShapeTangent = false;
    SampledTexture2DPtr _blendShapeTexture{nullptr};
    
    size_t _vertexCount = 0;
    bool _accessible = true;
    std::vector<float> _vertices{};
    std::vector<uint32_t> _indices32{};
    std::vector<uint16_t> _indices16{};
    wgpu::IndexFormat _indicesFormat;
    int _vertexChangeFlag;
    size_t _elementCount;
    std::vector<wgpu::VertexAttribute> _vertexAttribute{};
    
    std::vector<Vector3F> _positions;
    std::vector<Vector3F> _normals;
    std::vector<Color> _colors;
    std::vector<Vector4F> _tangents;
    std::vector<Vector2F> _uv;
    std::vector<Vector2F> _uv1;
    std::vector<Vector2F> _uv2;
    std::vector<Vector2F> _uv3;
    std::vector<Vector2F> _uv4;
    std::vector<Vector2F> _uv5;
    std::vector<Vector2F> _uv6;
    std::vector<Vector2F> _uv7;
    std::vector<Vector4F> _boneWeights;
    std::vector<Vector4F> _boneIndices;
};

}

#endif /* model_mesh_hpp */
