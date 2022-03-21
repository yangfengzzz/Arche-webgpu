//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_view_hpp
#define scene_view_hpp

#include "view_controllable.h"
//#include "OvEditor/Core/GizmoBehaviour.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class SceneView : public ViewControllable {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    SceneView(const std::string &p_title,
              bool p_opened,
              const PanelWindowSettings &p_windowSettings);
    
    /**
     * Update the scene view
     */
    virtual void update(float p_deltaTime) override;
    
    /**
     * Custom implementation of the render method
     */
    virtual void _render_Impl() override;
    
    /**
     * Render the actual scene
     * @param p_defaultRenderState p_defaultRenderState
     */
    void renderScene(uint8_t p_defaultRenderState);
    
    /**
     * Render the scene for actor picking (Using unlit colors)
     */
    void renderSceneForActorPicking();
    
    /**
     * Render the scene for actor picking and handle the logic behind it
     */
    void handleActorPicking();
    
private:
    //    OvCore::SceneSystem::SceneManager& m_sceneManager;
    //    OvRendering::Buffers::Framebuffer m_actorPickingFramebuffer;
    //    OvEditor::Core::GizmoBehaviour m_gizmoOperations;
    //    OvEditor::Core::EGizmoOperation m_currentOperation = OvEditor::Core::EGizmoOperation::TRANSLATE;
    
    //    std::optional<std::reference_wrapper<OvCore::ECS::Actor>> m_highlightedActor;
    //    std::optional<OvEditor::Core::GizmoBehaviour::EDirection> m_highlightedGizmoDirection;
};


}
}
}
#endif /* scene_view_hpp */
