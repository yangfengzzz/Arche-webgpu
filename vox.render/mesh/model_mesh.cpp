//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/model_mesh.h"

#include "vox.render/shader/shader_common.h"

namespace vox {
bool ModelMesh::accessible() const { return _accessible; }

size_t ModelMesh::vertexCount() const { return _vertexCount; }

ModelMesh::ModelMesh(wgpu::Device &device) : _device(device) {}

void ModelMesh::setPositions(const std::vector<Vector3F> &positions) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    auto count = positions.size();
    _positions = positions;
    _vertexChangeFlag |= ValueChanged::Position;

    if (_vertexCount != count) {
        _vertexCount = count;
    }
}

void ModelMesh::setPositions(const std::vector<float> &positions) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    auto count = positions.size() / 3;
    _f32positions = positions;
    _vertexChangeFlag |= ValueChanged::Position;

    if (_vertexCount != count) {
        _vertexCount = count;
    }
}

const std::vector<Vector3F> &ModelMesh::positions() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    return _positions;
}

void ModelMesh::setNormals(const std::vector<Vector3F> &normals) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (normals.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Normal;
    _normals = normals;
}

void ModelMesh::setNormals(const std::vector<float> &normals) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (normals.size() / 3 != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Normal;
    _f32normals = normals;
}

const std::vector<Vector3F> &ModelMesh::normals() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _normals;
}

void ModelMesh::setColors(const std::vector<Color> &colors) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (colors.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Color;
    _colors = colors;
}

void ModelMesh::setColors(const std::vector<float> &colors) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (colors.size() / 4 != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Color;
    _f32colors = colors;
}

const std::vector<Color> &ModelMesh::colors() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _colors;
}

void ModelMesh::setJointWeights(const std::vector<float> &value) {
    _vertexChangeFlag |= ValueChanged::BoneWeight;
    _boneWeights = value;
}

const std::vector<float> &ModelMesh::jointWeights() const {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _boneWeights;
}

void ModelMesh::setJointIndices(const std::vector<float> &value) {
    _vertexChangeFlag |= ValueChanged::BoneIndex;
    _boneIndices = value;
}

const std::vector<float> &ModelMesh::jointIndices() const {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _boneIndices;
}

void ModelMesh::setTangents(const std::vector<Vector4F> &tangents) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (tangents.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Tangent;
    _tangents = tangents;
}

void ModelMesh::setTangents(const std::vector<float> &tangents) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (tangents.size() / 4 != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    _vertexChangeFlag |= ValueChanged::Tangent;
    _f32tangents = tangents;
}

const std::vector<Vector4F> &ModelMesh::tangents() {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    return _tangents;
}

void ModelMesh::setUVs(const std::vector<Vector2F> &uv, int channelIndex) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (uv.size() != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    switch (channelIndex) {
        case 0:
            _vertexChangeFlag |= ValueChanged::UV;
            _uv = uv;
            break;
        case 1:
            _vertexChangeFlag |= ValueChanged::UV1;
            _uv1 = uv;
            break;
        case 2:
            _vertexChangeFlag |= ValueChanged::UV2;
            _uv2 = uv;
            break;
        case 3:
            _vertexChangeFlag |= ValueChanged::UV3;
            _uv3 = uv;
            break;
        case 4:
            _vertexChangeFlag |= ValueChanged::UV4;
            _uv4 = uv;
            break;
        case 5:
            _vertexChangeFlag |= ValueChanged::UV5;
            _uv5 = uv;
            break;
        case 6:
            _vertexChangeFlag |= ValueChanged::UV6;
            _uv6 = uv;
            break;
        case 7:
            _vertexChangeFlag |= ValueChanged::UV7;
            _uv7 = uv;
            break;
        default:
            assert(false && "The index of channel needs to be in range [0 - 7].");
    }
}

void ModelMesh::setUVs(const std::vector<float> &uv, int channelIndex) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    if (uv.size() / 2 != _vertexCount) {
        assert(false && "The array provided needs to be the same size as vertex count.");
    }

    switch (channelIndex) {
        case 0:
            _vertexChangeFlag |= ValueChanged::UV;
            _f32uv = uv;
            break;
        case 1:
            _vertexChangeFlag |= ValueChanged::UV1;
            _f32uv1 = uv;
            break;
        case 2:
            _vertexChangeFlag |= ValueChanged::UV2;
            _f32uv2 = uv;
            break;
        case 3:
            _vertexChangeFlag |= ValueChanged::UV3;
            _f32uv3 = uv;
            break;
        case 4:
            _vertexChangeFlag |= ValueChanged::UV4;
            _f32uv4 = uv;
            break;
        case 5:
            _vertexChangeFlag |= ValueChanged::UV5;
            _f32uv5 = uv;
            break;
        case 6:
            _vertexChangeFlag |= ValueChanged::UV6;
            _f32uv6 = uv;
            break;
        case 7:
            _vertexChangeFlag |= ValueChanged::UV7;
            _f32uv7 = uv;
            break;
        default:
            assert(false && "The index of channel needs to be in range [0 - 7].");
    }
}

const std::vector<Vector2F> &ModelMesh::uvs(int channelIndex) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    switch (channelIndex) {
        case 0:
            return _uv;
        case 1:
            return _uv1;
        case 2:
            return _uv2;
        case 3:
            return _uv3;
        case 4:
            return _uv4;
        case 5:
            return _uv5;
        case 6:
            return _uv6;
        case 7:
            return _uv7;
        default:
            assert(false && "The index of channel needs to be in range [0 - 7].");
            throw std::exception();
    }
}

void ModelMesh::setIndices(const std::vector<uint32_t> &indices) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    _indicesFormat = wgpu::IndexFormat::Uint32;
    _indices32 = indices;
}

void ModelMesh::setIndices(const std::vector<uint16_t> &indices) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }
    _indicesFormat = wgpu::IndexFormat::Uint16;
    _indices16 = indices;
}

void ModelMesh::uploadData(bool noLongerAccessible) {
    if (!_accessible) {
        assert(false && "Not allowed to access data while accessible is false.");
    }

    auto vertexLayouts = _updateVertexLayouts();
    _setVertexLayouts({vertexLayouts});
    _vertexChangeFlag = ValueChanged::All;

    auto vertexFloatCount = _elementCount * _vertexCount;
    auto vertices = std::vector<float>(vertexFloatCount);
    _updateVertices(vertices);

    auto newVertexBuffer = Buffer(_device, vertices.data(), vertices.size() * sizeof(float),
                                  wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);
    _setVertexBufferBinding(0, newVertexBuffer);

    if (_indicesFormat == wgpu::IndexFormat::Uint16) {
        auto newIndexBuffer = Buffer(_device, _indices16.data(), _indices16.size() * sizeof(uint16_t),
                                     wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst);
        _setIndexBufferBinding(IndexBufferBinding(newIndexBuffer, _indicesFormat));
    } else if (_indicesFormat == wgpu::IndexFormat::Uint32) {
        auto newIndexBuffer = Buffer(_device, _indices32.data(), _indices32.size() * sizeof(uint32_t),
                                     wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst);
        _setIndexBufferBinding(IndexBufferBinding(newIndexBuffer, _indicesFormat));
    }

    if (noLongerAccessible) {
        _accessible = false;
        _releaseCache();
    }
}

wgpu::VertexBufferLayout ModelMesh::_updateVertexLayouts() {
    _vertexAttribute.resize(1);
    _vertexAttribute[0] = wgpu::VertexAttribute{wgpu::VertexFormat::Float32x3, 0, (uint32_t)Attributes::POSITION};

    size_t offset = 12;
    size_t elementCount = 3;
    if (!_normals.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x3, offset, (uint32_t)Attributes::NORMAL});
        offset += 12;
        elementCount += 3;
    }
    if (!_colors.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x4, offset, (uint32_t)Attributes::COLOR_0});
        offset += 16;
        elementCount += 4;
    }
    if (!_boneWeights.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x4, offset, (uint32_t)Attributes::WEIGHTS_0});
        offset += 16;
        elementCount += 4;
    }
    if (!_boneIndices.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Uint16x4, offset, (uint32_t)Attributes::JOINTS_0});
        offset += 8;
        elementCount += 2;
    }
    if (!_tangents.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x4, offset, (uint32_t)Attributes::TANGENT});
        offset += 16;
        elementCount += 4;
    }
    if (!_uv.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_0});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv1.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_1});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv2.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_2});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv3.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_3});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv4.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_4});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv5.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_5});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv6.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_6});
        offset += 8;
        elementCount += 2;
    }
    if (!_uv7.empty()) {
        _vertexAttribute.push_back(
                wgpu::VertexAttribute{wgpu::VertexFormat::Float32x2, offset, (uint32_t)Attributes::UV_7});
        offset += 8;
        elementCount += 2;
    }

    wgpu::VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.arrayStride = elementCount * 4;
    vertexBufferLayout.attributes = _vertexAttribute.data();
    vertexBufferLayout.attributeCount = static_cast<uint32_t>(_vertexAttribute.size());
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

    _elementCount = elementCount;
    return vertexBufferLayout;
}

void ModelMesh::_updateVertices(std::vector<float> &vertices) {
    if ((_vertexChangeFlag & ValueChanged::Position) != 0) {
        if (!_positions.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i;
                const auto &position = _positions[i];
                vertices[start] = position.x;
                vertices[start + 1] = position.y;
                vertices[start + 2] = position.z;
            }
        }

        if (!_f32positions.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i;
                vertices[start] = _f32positions[i * 3];
                vertices[start + 1] = _f32positions[i * 3 + 1];
                vertices[start + 2] = _f32positions[i * 3 + 2];
            }
        }
    }

    size_t offset = 3;
    bool updated;

    if ((_vertexChangeFlag & ValueChanged::Normal) != 0) {
        updated = false;
        if (!_normals.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &normal = _normals[i];
                vertices[start] = normal.x;
                vertices[start + 1] = normal.y;
                vertices[start + 2] = normal.z;
            }
            updated = true;
        }

        if (!_f32normals.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _f32normals[i * 3];
                vertices[start + 1] = _f32normals[i * 3 + 1];
                vertices[start + 2] = _f32normals[i * 3 + 2];
            }
            updated = true;
        }

        if (updated) {
            offset += 3;
        }
    }

    if ((_vertexChangeFlag & ValueChanged::Color) != 0) {
        updated = false;
        if (!_colors.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &color = _colors[i];
                vertices[start] = color.r;
                vertices[start + 1] = color.g;
                vertices[start + 2] = color.b;
                vertices[start + 3] = color.a;
            }
            updated = true;
        }

        if (!_f32colors.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _f32colors[i * 4];
                vertices[start + 1] = _f32colors[i * 4 + 1];
                vertices[start + 2] = _f32colors[i * 4 + 2];
                vertices[start + 3] = _f32colors[i * 4 + 3];
            }
            updated = true;
        }

        if (updated) {
            offset += 4;
        }
    }

    if (!_boneWeights.empty()) {
        if (_vertexChangeFlag & ValueChanged::BoneWeight) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _boneWeights[i * 4];
                vertices[start + 1] = _boneWeights[i * 4 + 1];
                vertices[start + 2] = _boneWeights[i * 4 + 2];
                vertices[start + 3] = _boneWeights[i * 4 + 3];
            }
        }
        offset += 4;
    }

    if (!_boneIndices.empty()) {
        if (_vertexChangeFlag & ValueChanged::BoneIndex) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _boneIndices[i * 4];
                vertices[start + 1] = _boneIndices[i * 4 + 1];
            }
        }
        offset += 2;
    }

    if ((_vertexChangeFlag & ValueChanged::Tangent) != 0) {
        updated = false;
        if (!_tangents.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &tangent = _tangents[i];
                vertices[start] = tangent.x;
                vertices[start + 1] = tangent.y;
                vertices[start + 2] = tangent.z;
            }
            updated = true;
        }

        if (!_f32tangents.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _f32tangents[i * 3];
                vertices[start + 1] = _f32tangents[i * 3 + 1];
                vertices[start + 2] = _f32tangents[i * 3 + 2];
            }
            updated = true;
        }

        if (updated) {
            offset += 4;
        }
    }

    if ((_vertexChangeFlag & ValueChanged::UV) != 0) {
        updated = false;
        if (!_uv.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
            updated = true;
        }

        if (!_f32uv.empty()) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                vertices[start] = _f32uv[i * 2];
                vertices[start + 1] = _f32uv[i * 2 + 1];
            }
            updated = true;
        }

        if (updated) {
            offset += 2;
        }
    }

    if (!_uv1.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV1) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv1[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv2.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV2) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv2[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv3.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV3) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv3[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv4.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV4) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv4[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv5.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV5) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv5[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv6.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV6) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv6[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }
    if (!_uv7.empty()) {
        if ((_vertexChangeFlag & ValueChanged::UV7) != 0) {
            for (size_t i = 0; i < _vertexCount; i++) {
                auto start = _elementCount * i + offset;
                const auto &uv = _uv7[i];
                vertices[start] = uv.x;
                vertices[start + 1] = uv.y;
            }
        }
        offset += 2;
    }

    _vertexChangeFlag = 0;
}

void ModelMesh::_releaseCache() {
    _vertices.clear();
    _positions.clear();
    _tangents.clear();
    _normals.clear();
    _colors.clear();
    _uv.clear();
    _uv1.clear();
    _uv2.clear();
    _uv3.clear();
    _uv4.clear();
    _uv5.clear();
    _uv6.clear();
    _uv7.clear();
}

}  // namespace vox
