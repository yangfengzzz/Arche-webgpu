//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_renderer_hpp
#define mesh_renderer_hpp

#include "renderer.h"

namespace vox {
class MeshRenderer: public Renderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    explicit MeshRenderer(Entity *entity);

    /**
     * Mesh assigned to the renderer.
     */
    void setMesh(const MeshPtr &mesh);
    
    MeshPtr mesh();
    
private:
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
public:
    /**
     * Serialize the component
     */
    void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Deserialize the component
     */
    void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;
    
private:
    MeshPtr _mesh;
    std::unique_ptr<UpdateFlag> _meshUpdateFlag;
};

}

#endif /* mesh_renderer_hpp */
