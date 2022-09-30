//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/skinned_mesh_renderer.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/animation/animator.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_common.h"

namespace vox {
const std::string SkinnedMeshRenderer::_skinningMatrixProperty = "u_jointMatrix";

std::string SkinnedMeshRenderer::name() { return "SkinnedMeshRenderer"; }

SkinnedMeshRenderer::SkinnedMeshRenderer(Entity* entity) : MeshRenderer(entity) {}

bool SkinnedMeshRenderer::loadSkins(const std::string& filename) {
    LOGI("Loading meshes archive: {}", filename)
    vox::io::File file((fs::path::Get(fs::path::Type::ASSETS) + filename).c_str(), "rb");
    if (!file.opened()) {
        LOGE("Failed to open mesh file {}.", filename)
        return false;
    }
    vox::io::IArchive archive(&file);

    while (archive.TestTag<Skin>()) {
        _skins.resize(_skins.size() + 1);
        archive >> _skins.back();
    }

    // Computes the number of skinning matrices required to skin all meshes.
    // A mesh is skinned by only a subset of joints, so the number of skinning
    // matrices might be less that the number of skeleton joints.
    // Mesh::joint_remaps is used to know how to order skinning matrices. So
    // the number of matrices required is the size of joint_remaps.
    size_t num_skinning_matrices = 0;
    for (const Skin& skin : _skins) {
        num_skinning_matrices = std::max(num_skinning_matrices, skin.joint_remaps.size());
        _createMesh(skin);
    }
    // Allocates skinning matrices.
    _skinning_matrices.resize(num_skinning_matrices);
    if (!_meshes.empty()) {
        _meshUpdateFlag = _meshes[0]->registerUpdateFlag();
    }

    return true;
}

void SkinnedMeshRenderer::render(std::vector<RenderElement>& opaqueQueue,
                                 std::vector<RenderElement>& alphaTestQueue,
                                 std::vector<RenderElement>& transparentQueue) {
    if (!_meshes.empty()) {
        if (_meshUpdateFlag->flag) {
            const auto& vertexLayouts = _meshes[0]->vertexBufferLayouts();

            shaderData.removeDefine(HAS_UV);
            shaderData.removeDefine(HAS_NORMAL);
            shaderData.removeDefine(HAS_TANGENT);
            shaderData.removeDefine(HAS_VERTEXCOLOR);

            for (const auto& vertexLayout : vertexLayouts) {
                for (uint32_t j = 0, m = vertexLayout.attributeCount; j < m; j++) {
                    if (vertexLayout.attributes[j].shaderLocation == (uint32_t)Attributes::UV_0) {
                        shaderData.addDefine(HAS_UV);
                    }
                    if (vertexLayout.attributes[j].shaderLocation == (uint32_t)Attributes::NORMAL) {
                        shaderData.addDefine(HAS_NORMAL);
                    }
                    if (vertexLayout.attributes[j].shaderLocation == (uint32_t)Attributes::TANGENT) {
                        shaderData.addDefine(HAS_TANGENT);
                    }
                    if (vertexLayout.attributes[j].shaderLocation == (uint32_t)Attributes::COLOR_0) {
                        shaderData.addDefine(HAS_VERTEXCOLOR);
                    }
                }
            }
            _meshUpdateFlag->flag = false;
        }

        for (size_t i = 0; i < _meshes.size(); i++) {
            MaterialPtr material;
            if (i < _materials.size()) {
                material = _materials[i];
            } else {
                material = nullptr;
            }
            if (material != nullptr) {
                RenderElement element(this, _meshes[i], _meshes[i]->subMesh(), material);
                pushPrimitive(element, opaqueQueue, alphaTestQueue, transparentQueue);
            }
        }
    }
}

void SkinnedMeshRenderer::update(float deltaTime) {
    if (!_animator) {
        _animator = entity()->getComponent<Animator>();
    }

    if (_animator && !_skins.empty()) {
        // Builds skinning matrices, based on the output of the animation stage.
        // The mesh might not use (aka be skinned by) all skeleton joints. We
        // use the joint remapping table (available from the mesh object) to
        // reorder model-space matrices and build skinning ones.
        for (const Skin& skin : _skins) {
            for (size_t i = 0; i < skin.joint_remaps.size(); ++i) {
                _skinning_matrices[i] = _animator->models()[skin.joint_remaps[i]] * skin.inverse_bind_poses[i];
            }
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
    }
}

void SkinnedMeshRenderer::_createMesh(const Skin& skin) {
    int vertex_count = 0;
    for (const auto& part : skin.parts) {
        vertex_count += part.vertex_count();
    }
    std::vector<float> positions(vertex_count * Skin::Part::kPositionsCpnts);
    std::vector<float> normals(vertex_count * Skin::Part::kNormalsCpnts);
    std::vector<float> tangents(vertex_count * Skin::Part::kTangentsCpnts);
    std::vector<float> uvs(vertex_count * Skin::Part::kUVsCpnts);
    std::vector<float> joint_indices(vertex_count * 2);
    std::vector<float> joint_weights(vertex_count * 4);
    std::vector<float> colors(vertex_count * Skin::Part::kColorsCpnts);
    std::vector<uint16_t> indices(skin.triangle_indices.size());

    union {
        struct {
            uint16_t index1;
            uint16_t index2;
        } index;
        float f32index;
    } union_index{};

    vertex_count = 0;
    for (const auto& part : skin.parts) {
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
            } else {
                for (int j = 0; j < 4; ++j) {
                    joint_weights[vertex_count * 4 + i * 4 + j] = part.joint_weights[i * weight_influences_count + j];
                }
            }
        }

        for (int i = 0; i < part_vertex_count * Skin::Part::kColorsCpnts; ++i) {
            colors[vertex_count * Skin::Part::kColorsCpnts + i] = static_cast<float>(part.colors[i]) / 255.f;
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

    std::copy(skin.triangle_indices.begin(), skin.triangle_indices.end(), indices.begin());
    mesh->setIndices(indices);
    mesh->addSubMesh(0, static_cast<uint32_t>(indices.size()));
    mesh->uploadData(true);
    _meshes.emplace_back(mesh);
}

}  // namespace vox
