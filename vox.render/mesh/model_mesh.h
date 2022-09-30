//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/image.h"
#include "vox.render/mesh/mesh.h"
#include "vox.base/containers/vector.h"

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
        All = 0xffff
    };
};

/**
 * Mesh containing common vertex elements of the model.
 */
class ModelMesh : public Mesh {
public:
    /**
     * Whether to access data of the mesh.
     */
    [[nodiscard]] bool accessible() const;

    /**
     * Vertex count of current mesh.
     */
    [[nodiscard]] size_t vertexCount() const;

    /**
     * Create a model mesh.
     * @param device - Engine to which the mesh belongs
     */
    explicit ModelMesh(wgpu::Device &device);

public:
    /**
     * Get positions for the mesh.
     * @remarks Please call the setPositions() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector3F> &positions();

    /**
     * Set positions for the mesh.
     * @param positions - The positions for the mesh.
     */
    void setPositions(const std::vector<Vector3F> &positions);

    /**
     * Set positions for the mesh.
     * @param positions - The positions for the mesh.
     */
    void setPositions(const std::vector<float> &positions);

public:
    /**
     * Get normals for the mesh.
     * @remarks Please call the setNormals() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector3F> &normals();

    /**
     * Set per-vertex normals for the mesh.
     * @param normals - The normals for the mesh.
     */
    void setNormals(const std::vector<Vector3F> &normals);

    /**
     * Set per-vertex normals for the mesh.
     * @param normals - The normals for the mesh.
     */
    void setNormals(const std::vector<float> &normals);

public:
    /**
     * Get colors for the mesh.
     * @remarks Please call the setColors() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Color> &colors();

    /**
     * Set per-vertex colors for the mesh.
     * @param colors - The colors for the mesh.
     */
    void setColors(const std::vector<Color> &colors);

    /**
     * Set per-vertex colors for the mesh.
     * @param colors - The colors for the mesh.
     */
    void setColors(const std::vector<float> &colors);

public:
    /**
     * Set per-vertex bone weights for the mesh.
     * @param value - The bone weights for the mesh.
     */
    void setJointWeights(const std::vector<float> &value);

    /**
     * Get weights for the mesh.
     * @remarks Please call the setWeights() method after modification to ensure that the modification takes effect.
     */
    [[nodiscard]] const std::vector<float> &jointWeights() const;

    /**
     * Set per-vertex bone indices for the mesh.
     * @param value - The bone indices for the mesh.
     */
    void setJointIndices(const std::vector<float> &value);

    /**
     * Get joints for the mesh.
     * @remarks Please call the setJointIndices() method after modification to ensure that the modification takes
     * effect.
     */
    [[nodiscard]] const std::vector<float> &jointIndices() const;

public:
    /**
     * Get tangents for the mesh.
     * @remarks Please call the setTangents() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector4F> &tangents();

    /**
     * Set per-vertex tangents for the mesh.
     * @param tangents - The tangents for the mesh.
     */
    void setTangents(const std::vector<Vector4F> &tangents);

    /**
     * Set per-vertex tangents for the mesh.
     * @param tangents - The tangents for the mesh.
     */
    void setTangents(const std::vector<float> &tangents);

public:
    /**
     * Set per-vertex uv for the mesh by channelIndex.
     * @param uv - The uv for the mesh.
     * @param channelIndex - The index of uv channels, in [0 ~ 7] range.
     */
    void setUVs(const std::vector<Vector2F> &uv, int channelIndex = 0);

    /**
     * Set per-vertex uv for the mesh by channelIndex.
     * @param uv - The uv for the mesh.
     * @param channelIndex - The index of uv channels, in [0 ~ 7] range.
     */
    void setUVs(const std::vector<float> &uv, int channelIndex = 0);

    /**
     * Get uv for the mesh by channelIndex.
     * @param channelIndex - The index of uv channels, in [0 ~ 7] range.
     * @remarks Please call the setUV() method after modification to ensure that the modification takes effect.
     */
    const std::vector<Vector2F> &uvs(int channelIndex = 0);

public:
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
     * @param noLongerAccessible - Whether to access data later. If true, you'll never access data anymore (free memory
     * cache)
     */
    void uploadData(bool noLongerAccessible);

private:
    wgpu::Device &_device;

    wgpu::VertexBufferLayout _updateVertexLayouts();

    void _updateVertices(std::vector<float> &vertices);

    void _releaseCache();

    bool _hasBlendShape = false;
    bool _useBlendShapeNormal = false;
    bool _useBlendShapeTangent = false;
    std::shared_ptr<Image> _blendShapeTexture{nullptr};

    size_t _vertexCount = 0;
    bool _accessible = true;
    std::vector<float> _vertices{};
    std::vector<uint32_t> _indices32{};
    std::vector<uint16_t> _indices16{};
    wgpu::IndexFormat _indicesFormat;
    int _vertexChangeFlag{};
    size_t _elementCount{};
    std::vector<wgpu::VertexAttribute> _vertexAttribute{};

    std::vector<Vector3F> _positions;
    std::vector<float> _f32positions;

    std::vector<Vector3F> _normals;
    std::vector<float> _f32normals;

    std::vector<Color> _colors;
    std::vector<float> _f32colors;

    std::vector<Vector4F> _tangents;
    std::vector<float> _f32tangents;

    std::vector<Vector2F> _uv;
    std::vector<float> _f32uv;

    std::vector<Vector2F> _uv1;
    std::vector<float> _f32uv1;

    std::vector<Vector2F> _uv2;
    std::vector<float> _f32uv2;

    std::vector<Vector2F> _uv3;
    std::vector<float> _f32uv3;

    std::vector<Vector2F> _uv4;
    std::vector<float> _f32uv4;

    std::vector<Vector2F> _uv5;
    std::vector<float> _f32uv5;

    std::vector<Vector2F> _uv6;
    std::vector<float> _f32uv6;

    std::vector<Vector2F> _uv7;
    std::vector<float> _f32uv7;

    std::vector<float> _boneWeights;
    std::vector<float> _boneIndices;
};
using ModelMeshPtr = std::shared_ptr<ModelMesh>;

}  // namespace vox
