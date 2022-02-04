//
//  gpu_skinned_mesh_renderer.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/16.
//

#include "gpu_skinned_mesh_renderer.h"
#include "entity.h"
#include "scene.h"
#include "shader/shader.h"

namespace vox {
GPUSkinnedMeshRenderer::GPUSkinnedMeshRenderer(Entity *entity) :
MeshRenderer(entity),
_jointMatrixProperty(Shader::createProperty("u_jointMatrix", ShaderDataGroup::Renderer)) {
}

GPUSkinnedMeshRenderer::SkinPtr GPUSkinnedMeshRenderer::skin() {
    return _skin;
}

void GPUSkinnedMeshRenderer::setSkin(const SkinPtr &skin) {
    _skin = skin;
}

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
        memcpy(matrixPalette->contents(), jointMatrix.data(), jointMatrix.size() * sizeof(float));
        shaderData.setData(_jointMatrixProperty, matrixPalette);
    }
}

void GPUSkinnedMeshRenderer::_initJoints() {
    jointMatrix.resize(_skin->joints.size() * 16);
    matrixPalette = std::shared_ptr<MTL::Buffer>(scene()->device()->newBufferWithLength(jointMatrix.size() * sizeof(float)));
    shaderData.enableMacro(HAS_SKIN);
}


}
