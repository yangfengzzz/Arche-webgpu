//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh.h"

namespace vox {
uint32_t Mesh::instanceCount() const { return _instanceCount; }

SubMesh* Mesh::subMesh() {
    if (!_subMeshes.empty()) {
        return &_subMeshes[0];
    } else {
        return nullptr;
    }
}

const SubMesh* Mesh::subMesh() const {
    if (!_subMeshes.empty()) {
        return &_subMeshes[0];
    } else {
        return nullptr;
    }
}

const std::vector<SubMesh>& Mesh::subMeshes() const { return _subMeshes; }

void Mesh::addSubMesh(SubMesh subMesh) { _subMeshes.push_back(subMesh); }

void Mesh::addSubMesh(uint32_t start, uint32_t count, wgpu::PrimitiveTopology topology) {
    _subMeshes.emplace_back(start, count, topology);
}

void Mesh::clearSubMesh() { _subMeshes.clear(); }

std::unique_ptr<UpdateFlag> Mesh::registerUpdateFlag() { return _updateFlagManager.registration(); }

void Mesh::_setVertexLayouts(const std::vector<wgpu::VertexBufferLayout>& layouts) {
    _clearVertexLayouts();
    for (const auto& layout : layouts) {
        _addVertexLayout(layout);
    }
}

void Mesh::_setVertexBufferBinding(size_t index, const Buffer& binding) {
    if (_vertexBufferBindings.size() > index) {
        _vertexBufferBindings[index] = binding;
    } else {
        _vertexBufferBindings.insert(_vertexBufferBindings.begin() + index, binding);
    }
}

void Mesh::_setIndexBufferBinding(const std::optional<IndexBufferBinding>& binding) {
    if (binding.has_value()) {
        _indexBufferBinding = binding;
    } else {
        _indexBufferBinding = std::nullopt;
    }
}

void Mesh::_clearVertexLayouts() { _vertexBufferLayouts.clear(); }

void Mesh::_addVertexLayout(const wgpu::VertexBufferLayout& layout) {
    _vertexBufferLayouts.push_back(layout);
    _updateFlagManager.distribute();
}

const std::vector<wgpu::VertexBufferLayout>& Mesh::vertexBufferLayouts() const { return _vertexBufferLayouts; }

const std::vector<std::optional<Buffer>>& Mesh::vertexBufferBindings() const { return _vertexBufferBindings; }

const std::optional<IndexBufferBinding>& Mesh::indexBufferBinding() const { return _indexBufferBinding; }

}  // namespace vox
