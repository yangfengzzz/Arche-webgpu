//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef game_view_hpp
#define game_view_hpp

#include "view.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class GameView : public View {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    GameView(const std::string &p_title,
             bool p_opened,
             const PanelWindowSettings &p_windowSettings);
    
    /**
     * Update the view
     * @param p_deltaTime p_deltaTimes
     */
    virtual void update(float p_deltaTime) override;
    
    /**
     * Custom implementation of the render method
     */
    virtual void _render_Impl() override;
    
    /**
     * Returns true if the game view has a camera
     */
    bool hasCamera() const;
    
    /**
     * Returns the game view camera frustum or nothing if the game isn't playing
     */
    //    std::optional<OvRendering::Data::Frustum> GetActiveFrustum() const;
    
private:
    //    OvCore::SceneSystem::SceneManager& _sceneManager;
    bool _hasCamera = false;
};


}
}
}
#endif /* game_view_hpp */
