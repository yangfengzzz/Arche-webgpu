//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/skinned_mesh_renderer.h"

#include "vox.render/animation/animator.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/shader/internal_variant_name.h"

namespace vox {
const std::string SkinnedMeshRenderer::_skinningMatrixProperty = "u_jointMatrix";

std::string SkinnedMeshRenderer::name() { return "SkinnedMeshRenderer"; }

SkinnedMeshRenderer::SkinnedMeshRenderer(Entity* entity) : MeshRenderer(entity) {}

void SkinnedMeshRenderer::setSkinnedMesh(const std::shared_ptr<Skin>& skin) {
    _skin = skin;
    // Computes the number of skinning matrices required to skin all meshes.
    // A mesh is skinned by only a subset of joints, so the number of skinning
    // matrices might be less that the number of skeleton joints.
    // Mesh::joint_remaps is used to know how to order skinning matrices. So
    // the number of matrices required is the size of joint_remaps.
    size_t num_skinning_matrices = skin->joint_remaps.size();
    // Allocates skinning matrices.
    _skinning_matrices.resize(num_skinning_matrices);
    _createMesh();
    _meshUpdateFlag = _mesh->registerUpdateFlag();
}

void SkinnedMeshRenderer::update(float deltaTime) {
    if (!_animator) {
        _animator = entity()->getComponent<Animator>();
    }

    if (_animator && _skin) {
        // Builds skinning matrices, based on the output of the animation stage.
        // The mesh might not use (aka be skinned by) all skeleton joints. We
        // use the joint remapping table (available from the mesh object) to
        // reorder model-space matrices and build skinning ones.
        for (size_t i = 0; i < _skin->joint_remaps.size(); ++i) {
            _skinning_matrices[i] = _animator->models()[_skin->joint_remaps[i]] * _skin->inverse_bind_poses[i];
        }
        shaderData.setData(_skinningMatrixProperty, _skinning_matrices);
        shaderData.addDefine(JOINTS_COUNT + std::to_string(_skinning_matrices.size()));
        shaderData.addDefine(HAS_SKIN);
    } else {
        shaderData.removeDefine(HAS_SKIN);
    }
}

void SkinnedMeshRenderer::_updateBounds(BoundingBox3F& worldBounds) {
    if (_animator) {
        _animator->computeSkeletonBounds(worldBounds);
        const auto worldMatrix = _entity->transform->worldMatrix();
        worldBounds.transform(worldMatrix);
    } else {
        worldBounds.lower_corner.set(-std::numeric_limits<float>::max());
        worldBounds.upper_corner.set(std::numeric_limits<float>::max());
    }
}

void SkinnedMeshRenderer::_createMesh() {
    int vertex_count = 0;
    for (const auto& part : _skin->parts) {
        vertex_count += part.vertex_count();
    }
    std::vector<float> positions(vertex_count * Skin::Part::kPositionsCpnts);
    std::vector<float> normals(vertex_count * Skin::Part::kNormalsCpnts);
    std::vector<float> tangents(vertex_count * Skin::Part::kTangentsCpnts);
    std::vector<float> uvs(vertex_count * Skin::Part::kUVsCpnts);
    std::vector<float> joint_indices(vertex_count * 2);
    std::vector<float> joint_weights(vertex_count * 4);
    std::vector<float> colors(vertex_count * Skin::Part::kColorsCpnts);
    std::vector<uint16_t> indices(size_t(std::ceil(float(_skin->triangle_indices.size()) / 4.0)) * 4);

    union {
        struct {
            uint16_t index1;
            uint16_t index2;
        } index;
        float f32index;
    } union_index{};

    vertex_count = 0;
    for (const auto& part : _skin->parts) {
        int part_vertex_count = part.vertex_count();
        int part_influences_count = part.influences_count();
        int weight_influences_count = part_influences_count - 1;
        std::copy(part.positions.begin(), part.positions.end(),
                  positions.begin() + vertex_count * Skin::Part::kPositionsCpnts);
        std::copy(part.normals.begin(), part.normals.end(), normals.begin() + vertex_count * Skin::Part::kNormalsCpnts);
        std::copy(part.tangents.begin(), part.tangents.end(),
                  tangents.begin() + vertex_count * Skin::Part::kTangentsCpnts);
        std::copy(part.uvs.begin(), part.uvs.end(), uvs.begin() + vertex_count * Skin::Part::kUVsCpnts);

        for (int i = 0; i < part_vertex_count; ++i) {
            uint16_t index[4];
            for (int j = 0; j < 4; ++j) {
                if (j < part_influences_count) {
                    index[j] = part.joint_indices[i * part_influences_count + j];
                } else {
                    index[j] = 0;
                }
            }

            union_index.index.index1 = index[0];
            union_index.index.index2 = index[1];
            joint_indices[vertex_count * 2 + i * 2] = union_index.f32index;
            union_index.index.index1 = index[1];
            union_index.index.index2 = index[2];
            joint_indices[vertex_count * 2 + i * 2 + 1] = union_index.f32index;
        }

        for (int i = 0; i < part_vertex_count; ++i) {
            if (weight_influences_count == 0) {
                joint_weights[vertex_count * 4 + i * 4] = 1.f;
            } else if (weight_influences_count == 1) {
                joint_weights[vertex_count * 4 + i * 4] = part.joint_weights[i * weight_influences_count];
                joint_weights[vertex_count * 4 + i * 4 + 1] = 1.f - part.joint_weights[i * weight_influences_count];
            } else if (weight_influences_count == 2) {
                joint_weights[vertex_count * 4 + i * 4] = part.joint_weights[i * weight_influences_count];
                joint_weights[vertex_count * 4 + i * 4 + 1] = part.joint_weights[i * weight_influences_count + 1];
                joint_weights[vertex_count * 4 + i * 4 + 2] =
                        1.f - joint_weights[vertex_count * 4 + i * 4] - joint_weights[vertex_count * 4 + i * 4 + 1];
            } else if (weight_influences_count == 3) {
                joint_weights[vertex_count * 4 + i * 4] = part.joint_weights[i * weight_influences_count];
                joint_weights[vertex_count * 4 + i * 4 + 1] = part.joint_weights[i * weight_influences_count + 1];
                joint_weights[vertex_count * 4 + i * 4 + 2] = part.joint_weights[i * weight_influences_count + 2];
                joint_weights[vertex_count * 4 + i * 4 + 3] = 1.f - joint_weights[vertex_count * 4 + i * 4] -
                                                              joint_weights[vertex_count * 4 + i * 4 + 1] -
                                                              joint_weights[vertex_count * 4 + i * 4 + 2];
            } else if (weight_influences_count == -1) {
                joint_weights[vertex_count * 4 + i * 4] = 0.f;
                joint_weights[vertex_count * 4 + i * 4 + 1] = 0.f;
                joint_weights[vertex_count * 4 + i * 4 + 2] = 0.f;
                joint_weights[vertex_count * 4 + i * 4 + 3] = 0.f;
            } else {
                for (int j = 0; j < 4; ++j) {
                    joint_weights[vertex_count * 4 + i * 4 + j] = part.joint_weights[i * weight_influences_count + j];
                }
            }
        }

        for (int i = 0; i < part_vertex_count * Skin::Part::kColorsCpnts; ++i) {
            if (i >= part.colors.size()) {
                colors[vertex_count * Skin::Part::kColorsCpnts + i] = 1.0;
            } else {
                colors[vertex_count * Skin::Part::kColorsCpnts + i] = static_cast<float>(part.colors[i]) / 255.f;
            }
        }

        vertex_count += part_vertex_count;
    }

    auto mesh = MeshManager::GetSingleton().LoadModelMesh();
    mesh->setPositions(positions);
    mesh->setNormals(normals);
    mesh->setTangents(tangents);
    mesh->setUVs(uvs, 0);
    mesh->setJointIndices(joint_indices);
    mesh->setJointWeights(joint_weights);
    mesh->setColors(colors);

    std::copy(_skin->triangle_indices.begin(), _skin->triangle_indices.end(), indices.begin());
    mesh->setIndices(indices);
    mesh->addSubMesh(0, static_cast<uint32_t>(_skin->triangle_indices.size()));
    mesh->uploadData(true);
    _mesh = mesh;
}

}  // namespace vox
