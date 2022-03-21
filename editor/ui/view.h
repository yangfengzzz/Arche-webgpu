//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef view_hpp
#define view_hpp

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/visual/image.h"
#include "vector3.h"
#include "quaternion.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
/**
 * Base class for any view
 */
class View : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    View(const std::string &p_title,
         bool p_opened,
         const PanelWindowSettings &p_windowSettings);
    
    /**
     * Update the view
     * @param p_deltaTime p_deltaTime
     */
    virtual void update(float p_deltaTime);
    
    /**
     * Custom implementation of the draw method
     */
    void _draw_Impl() override;
    
    /**
     * Custom implementation of the render method to define in dervied classes
     */
    virtual void _render_Impl() = 0;
    
    /**
     * Render the view
     */
    void render();
    
    /**
     * Defines the camera position
     * @param p_position p_position
     */
    void setCameraPosition(const Vector3F &p_position);
    
    /**
     * Defines the camera rotation
     * @param p_rotation p_rotation
     */
    void setCameraRotation(const QuaternionF &p_rotation);
    
    /**
     * Returns the camera position
     */
    const Vector3F &cameraPosition() const;
    
    /**
     * Returns the camera rotation
     */
    const QuaternionF &cameraRotation() const;
    
    /**
     * Returns the camera used by this view
     */
    // Camera& camera();
    
    /**
     * Returns the size of the panel ignoring its titlebar height
     */
    std::pair<uint16_t, uint16_t> safeSize() const;
    
    /**
     * Returns the grid color of the view
     */
    const Vector3F &gridColor() const;
    
    /**
     * Defines the grid color of the view
     * @param p_color p_color
     */
    void setGridColor(const Vector3F &p_color);
    
    /**
     * Fill the UBO using the view settings
     */
    void fillEngineUBO();
    
protected:
    /**
     * Update camera matrices
     */
    void prepareCamera();
    
protected:
    //    OvEditor::Core::EditorRenderer& m_editorRenderer;
    //    OvRendering::LowRenderer::Camera m_camera;
    Vector3F _cameraPosition;
    QuaternionF _cameraRotation;
    ::vox::ui::Image *image;
    
    Vector3F m_gridColor = {0.176f, 0.176f, 0.176f};
    
    //    OvRendering::Buffers::Framebuffer m_fbo;
};


}
}
}
#endif /* view_hpp */
