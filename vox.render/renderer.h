//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef renderer_hpp
#define renderer_hpp

#include "component.h"
#include "shader/shader_data.h"
#include "rendering/render_element.h"
#include "bounding_box3.h"
#include "matrix4x4.h"
#include "update_flag.h"

namespace vox {
/**
 * Renderable component.
 */
class Renderer : public Component {
public:
    struct RendererData {
        Matrix4x4F u_localMat;
        Matrix4x4F u_modelMat;
        Matrix4x4F u_MVMat;
        Matrix4x4F u_MVPMat;
        Matrix4x4F u_MVInvMat;
        Matrix4x4F u_normalMat;
    };
    
    /** ShaderData related to renderer. */
    ShaderData shaderData;
    // @ignoreClone
    /** Whether it is clipped by the frustum, needs to be turned on camera.enableFrustumCulling. */
    bool isCulled = false;
    
    /** Set whether the renderer to receive shadows. */
    bool receiveShadow = false;
    /** Set whether the renderer to cast shadows. */
    bool castShadow = false;
    
    /**
     * Material count.
     */
    size_t materialCount();
    
    /**
     * The bounding volume of the renderer.
     */
    BoundingBox3F bounds();
    
    explicit Renderer(Entity *entity);
    
public:
    /**
     * Get the first instance material by index.
     * @remarks Calling this function for the first time after the material is set will create an instance material to ensure that it is unique to the renderer.
     * @param index - Material index
     * @returns Instance material
     */
    MaterialPtr getInstanceMaterial(size_t index = 0);
    
    /**
     * Get the first material by index.
     * @param index - Material index
     * @returns Material
     */
    MaterialPtr getMaterial(size_t index = 0);
    
    /**
     * Set the first material.
     * @param material - The first material
     */
    void setMaterial(MaterialPtr material);
    
    /**
     * Set material by index.
     * @param index - Material index
     * @param material - The material
     */
    void setMaterial(size_t index, MaterialPtr material);
    
    /**
     * Get all instance materials.
     * @remarks Calling this function for the first time after the material is set will create an instance material to ensure that it is unique to the renderer.
     * @returns All instance materials
     */
    std::vector<MaterialPtr> getInstanceMaterials();
    
    /**
     * Get all materials.
     * @returns All materials
     */
    std::vector<MaterialPtr> getMaterials();
    
    /**
     * Set all materials.
     * @param materials - All materials
     */
    void setMaterials(const std::vector<MaterialPtr> &materials);
    
    void pushPrimitive(const RenderElement &element,
                       std::vector<RenderElement> &opaqueQueue,
                       std::vector<RenderElement> &alphaTestQueue,
                       std::vector<RenderElement> &transparentQueue);
    
    void setDistanceForSort(float dist);
    
    float distanceForSort();
    
    void updateShaderData(const Matrix4x4F& viewMat,
                          const Matrix4x4F& projMat);
    
protected:
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _onDestroy() override;
    
    virtual void _render(std::vector<RenderElement> &opaqueQueue,
                         std::vector<RenderElement> &alphaTestQueue,
                         std::vector<RenderElement> &transparentQueue) = 0;
    
    virtual void _updateBounds(BoundingBox3F &worldBounds) {
    }
    
    virtual void update(float deltaTime) {
    }
    
protected:
    MaterialPtr _createInstanceMaterial(const MaterialPtr &material, size_t index);
    
    std::vector<std::shared_ptr<Material>> _materials;
    
private:
    friend class ComponentsManager;
    
    float _distanceForSort = 0;
    ssize_t _rendererIndex = -1;
    
    RendererData _rendererData;
    ShaderProperty _rendererProperty;
    
    std::unique_ptr<UpdateFlag> _transformChangeFlag;
    // @deepClone
    BoundingBox3F _bounds = BoundingBox3F();
    // @ignoreClone
    Matrix4x4F _mvMatrix = Matrix4x4F();
    // @ignoreClone
    Matrix4x4F _mvpMatrix = Matrix4x4F();
    // @ignoreClone
    Matrix4x4F _mvInvMatrix = Matrix4x4F();
    // @ignoreClone
    Matrix4x4F _normalMatrix = Matrix4x4F();
    // @ignoreClone
    std::vector<bool> _materialsInstanced;
};


}

#endif /* renderer_hpp */
