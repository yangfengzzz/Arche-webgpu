//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/mesh_renderer.h"

#include "vox.render/entity.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_common.h"

namespace vox {
std::string MeshRenderer::name() { return "MeshRenderer"; }

MeshRenderer::MeshRenderer(Entity *entity) : Renderer(entity) {}

void MeshRenderer::setMesh(const MeshPtr &newValue) {
    auto &lastMesh = _mesh;
    if (lastMesh != newValue) {
        if (lastMesh != nullptr) {
            _meshUpdateFlag.reset();
        }
        if (newValue != nullptr) {
            _meshUpdateFlag = newValue->registerUpdateFlag();
        }
        _mesh = newValue;
    }
}

MeshPtr MeshRenderer::mesh() { return _mesh; }

void MeshRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                           std::vector<RenderElement> &alphaTestQueue,
                           std::vector<RenderElement> &transparentQueue) {
    if (_mesh != nullptr) {
        if (_meshUpdateFlag->flag) {
            const auto &vertexLayouts = _mesh->vertexBufferLayouts();

            shaderData.removeDefine(HAS_UV);
            shaderData.removeDefine(HAS_NORMAL);
            shaderData.removeDefine(HAS_TANGENT);
            shaderData.removeDefine(HAS_VERTEXCOLOR);

            for (const auto &vertexLayout : vertexLayouts) {
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

        auto &subMeshes = _mesh->subMeshes();
        for (size_t i = 0; i < subMeshes.size(); i++) {
            MaterialPtr material;
            if (i < _materials.size()) {
                material = _materials[i];
            } else {
                material = nullptr;
            }
            if (material != nullptr) {
                RenderElement element(this, _mesh, &subMeshes[i], material);
                pushPrimitive(element, opaqueQueue, alphaTestQueue, transparentQueue);
            }
        }
    } else {
        assert(false && "mesh is nullptr.");
    }
}

void MeshRenderer::_updateBounds(BoundingBox3F &worldBounds) {
    if (_mesh != nullptr) {
        const auto localBounds = _mesh->bounds;
        const auto worldMatrix = _entity->transform->worldMatrix();
        worldBounds = localBounds.transform(worldMatrix);
    } else {
        worldBounds.lower_corner = Point3F(0, 0, 0);
        worldBounds.upper_corner = Point3F(0, 0, 0);
    }
}

// MARK: - Reflection
void MeshRenderer::onSerialize(nlohmann::json &data) {}

void MeshRenderer::onDeserialize(nlohmann::json &data) {}

void MeshRenderer::onInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
