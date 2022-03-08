//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_hpp
#define editor_hpp

#include "editor_application.h"
#include "gui_entry.h"

namespace vox {
class Editor : public EditorApplication {
public:
    void loadScene() override;
    
    void pickFunctor(Renderer *renderer, MeshPtr mesh) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
private:
    editor::GUIEntry* _entry{nullptr};
};
 
}

#endif /* editor_hpp */
