//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef view_controllable_hpp
#define view_controllable_hpp

#include "view.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class ViewControllable : public View {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     * @param p_enableFocusInputs p_enableFocusInputs
     */
    ViewControllable(const std::string& p_title,
                     bool p_opened,
                     const PanelWindowSettings& p_windowSettings,
                     bool p_enableFocusInputs = false);
    
    /**
     * Update the controllable view (Handle inputs)
     * @param p_deltaTime p_deltaTime
     */
    virtual void update(float p_deltaTime) override;
    
    /**
     * Returns the camera controller of the controllable view
     */
//    OvEditor::Core::CameraController& cameraController();
    
protected:
//    OvEditor::Core::CameraController m_cameraController;
};

}
}
}
#endif /* view_controllable_hpp */
