//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/gpu_skinned_mesh_renderer.h"

#include "vox.render/entity.h"
#include "vox.render/scene.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
std::string GPUSkinnedMeshRenderer::name() { return "GPUSkinnedMeshRenderer"; }

GPUSkinnedMeshRenderer::GPUSkinnedMeshRenderer(Entity* entity)
    : MeshRenderer(entity), _jointMatrixProperty("u_jointMatrix") {}

GPUSkinnedMeshRenderer::SkinPtr GPUSkinnedMeshRenderer::skin() { return _skin; }

void GPUSkinnedMeshRenderer::setSkin(const SkinPtr& skin) { _skin = skin; }

void GPUSkinnedMeshRenderer::update(float deltaTime) {
    if (_skin) {
        if (!_hasInitJoints) {
            _initJoints();
            _hasInitJoints = true;
        }

        // Update join matrices
        auto m = entity()->transform->worldMatrix();
        auto inverseTransform = m.inverse();
        for (size_t i = 0; i < _skin->joints.size(); i++) {
            auto jointNode = _skin->joints[i];
            auto jointMat = jointNode->transform->worldMatrix() * _skin->inverseBindMatrices[i];
            jointMat = inverseTransform * jointMat;
            std::copy(jointMat.data(), jointMat.data() + 16, jointMatrix.data() + i * 16);
        }
        shaderData.setData(_jointMatrixProperty, jointMatrix);
        shaderData.addDefine(JOINTS_COUNT + std::to_string(_skin->joints.size()));
    }
}

void GPUSkinnedMeshRenderer::_initJoints() {
    jointMatrix.resize(_skin->joints.size() * 16);
    shaderData.removeDefine(HAS_SKIN);
}

// MARK: - Reflection
void GPUSkinnedMeshRenderer::onSerialize(nlohmann::json& data) {}

void GPUSkinnedMeshRenderer::onDeserialize(nlohmann::json& data) {}

void GPUSkinnedMeshRenderer::onInspector(ui::WidgetContainer& p_root) {}

}  // namespace vox
