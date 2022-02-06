//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef framebuffer_picker_app_hpp
#define framebuffer_picker_app_hpp

#include "editor_application.h"
#include <random>

namespace vox {
class FramebufferPickerApp : public EditorApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void pickFunctor(Renderer *renderer, MeshPtr mesh) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
private:
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};
 
}
#endif /* framebuffer_picker_app_hpp */
