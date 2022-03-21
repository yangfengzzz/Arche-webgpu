//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_renderer.h"
#include "graphics/mesh.h"
#include "shaderlib/wgsl_common.h"
#include "entity.h"

namespace vox {
MeshRenderer::MeshRenderer(Entity *entity) :
Renderer(entity) {
    
}

void MeshRenderer::setMesh(const MeshPtr &newValue) {
    auto &lastMesh = _mesh;
    if (lastMesh != newValue) {
        if (lastMesh != nullptr) {
            _meshUpdateFlag->destroy();
        }
        if (newValue != nullptr) {
            _meshUpdateFlag = newValue->registerUpdateFlag();
        }
        _mesh = newValue;
    }
}

MeshPtr MeshRenderer::mesh() {
    return _mesh;
}

void MeshRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                           std::vector<RenderElement> &alphaTestQueue,
                           std::vector<RenderElement> &transparentQueue) {
    if (_mesh != nullptr) {
        if (_meshUpdateFlag->flag) {
            const auto &vertexLayouts = _mesh->vertexBufferLayouts();
            
            shaderData.disableMacro(HAS_UV);
            shaderData.disableMacro(HAS_NORMAL);
            shaderData.disableMacro(HAS_TANGENT);
            shaderData.disableMacro(HAS_VERTEXCOLOR);
            
            for (size_t i = 0, n = vertexLayouts.size(); i < n; i++) {
                for (uint32_t j = 0, m = vertexLayouts[i].attributeCount; j < m; j++) {
                    if (vertexLayouts[i].attributes[j].shaderLocation == (uint32_t)Attributes::UV_0) {
                        shaderData.enableMacro(HAS_UV);
                    }
                    if (vertexLayouts[i].attributes[j].shaderLocation == (uint32_t)Attributes::Normal) {
                        shaderData.enableMacro(HAS_NORMAL);
                    }
                    if (vertexLayouts[i].attributes[j].shaderLocation == (uint32_t)Attributes::Tangent) {
                        shaderData.enableMacro(HAS_TANGENT);
                    }
                    if (vertexLayouts[i].attributes[j].shaderLocation == (uint32_t)Attributes::Color_0) {
                        shaderData.enableMacro(HAS_VERTEXCOLOR);
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
        worldBounds.lowerCorner = Point3F(0, 0, 0);
        worldBounds.upperCorner = Point3F(0, 0, 0);
    }
}

//MARK: - Reflection
void MeshRenderer::onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void MeshRenderer::onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void MeshRenderer::onInspector(ui::WidgetContainer& p_root) {
    
}


}
