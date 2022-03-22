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
    GameView(const std::string &p_title, bool p_opened,
             const PanelWindowSettings &p_windowSettings,
             RenderContext* renderContext, Scene* scene);
    
    /**
     * Custom implementation of the render method
     */
    void render(wgpu::CommandEncoder& commandEncoder) override;
    
private:
    Camera* _mainCamera{nullptr};
    Scene* _scene{nullptr};
};


}
}
}
#endif /* game_view_hpp */
