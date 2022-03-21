//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef material_editor_hpp
#define material_editor_hpp

#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/texts/text.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class MaterialEditor : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    MaterialEditor(const std::string &p_title,
                   bool p_opened,
                   const PanelWindowSettings &p_windowSettings);
    
    /**
     * Refresh the material editor
     */
    void refresh();
    
    /**
     * Defines the target material of the material editor
     * @param p_newTarget
     */
    //    void setTarget(OvCore::Resources::Material& p_newTarget);
    
    /**
     * Returns the target of the material editor
     */
    //    OvCore::Resources::Material* GetTarget() const;
    
    /**
     * Remove the target of the material editor (Clear the material editor)
     */
    void removeTarget();
    
    /**
     * Launch the preview of the currently targeted material
     */
    void preview();
    
    /**
     * Reset material
     */
    void reset();
    
private:
    void onMaterialDropped();
    
    void onShaderDropped();
    
    void createHeaderButtons();
    
    void createMaterialSelector();
    
    void createShaderSelector();
    
    void createMaterialSettings();
    
    void createShaderSettings();
    
    void generateShaderSettingsContent();
    
    void generateMaterialSettingsContent();
    
private:
    //    OvCore::Resources::Material* m_target        = nullptr;
    //    OvRendering::Resources::Shader* m_shader    = nullptr;
    
    Text *_targetMaterialText{nullptr};
    Text *_shaderText{nullptr};
    
    Event<> _materialDroppedEvent;
    Event<> _shaderDroppedEvent;
    
    Group *_settings{nullptr};
    Group *_materialSettings{nullptr};
    Group *_shaderSettings{nullptr};
    
    Columns<2> *_shaderSettingsColumns{nullptr};
    Columns<2> *_materialSettingsColumns{nullptr};
};


}
}
}
#endif /* material_editor_hpp */
