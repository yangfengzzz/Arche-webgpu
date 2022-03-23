//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "material_editor.h"

#include "ui/gui_drawer.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/layout/group_collapsable.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/buttons/button_small.h"
#include "ui/widgets/selection/color_edit.h"

namespace vox {
namespace editor {
namespace ui {
namespace {
void drawHybridVec3(WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data,
                    float p_step, float p_min, float p_max) {
    GUIDrawer::createTitle(p_root, p_name);
    
    auto &rightSide = p_root.createWidget<Group>();
    
    auto &xyzWidget = rightSide.createWidget<DragMultipleScalars<float, 3>>(GUIDrawer::getDataType<float>(), p_min, p_max,
                                                                            0.f, p_step, "", GUIDrawer::getFormat<float>());
    auto &xyzDispatcher = xyzWidget.addPlugin<DataDispatcher<std::array<float, 3>>>();
    xyzDispatcher.registerReference(reinterpret_cast<std::array<float, 3> &>(p_data));
    xyzWidget.lineBreak = false;
    
    auto &rgbWidget = rightSide.createWidget<ColorEdit>(false, Color{p_data.x, p_data.y, p_data.z});
    auto &rgbDispatcher = rgbWidget.addPlugin<DataDispatcher<Color>>();
    rgbDispatcher.registerReference(reinterpret_cast<Color &>(p_data));
    rgbWidget.enabled = false;
    rgbWidget.lineBreak = false;
    
    auto &xyzButton = rightSide.createWidget<ButtonSimple>("XYZ");
    xyzButton.idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    xyzButton.lineBreak = false;
    
    auto &rgbButton = rightSide.createWidget<ButtonSimple>("RGB");
    rgbButton.idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    
    xyzButton.clickedEvent += [&] {
        xyzWidget.enabled = true;
        rgbWidget.enabled = false;
    };
    
    rgbButton.clickedEvent += [&] {
        xyzWidget.enabled = false;
        rgbWidget.enabled = true;
    };
}

void drawHybridVec4(WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data,
                    float p_step, float p_min, float p_max) {
    GUIDrawer::createTitle(p_root, p_name);
    
    auto &rightSide = p_root.createWidget<Group>();
    
    auto &xyzWidget = rightSide.createWidget<DragMultipleScalars<float, 4>>(GUIDrawer::getDataType<float>(), p_min, p_max,
                                                                            0.f, p_step, "", GUIDrawer::getFormat<float>());
    auto &xyzDispatcher = xyzWidget.addPlugin<DataDispatcher<std::array<float, 4>>>();
    xyzDispatcher.registerReference(reinterpret_cast<std::array<float, 4> &>(p_data));
    xyzWidget.lineBreak = false;
    
    auto &rgbaWidget = rightSide.createWidget<ColorEdit>(true, Color{p_data.x, p_data.y, p_data.z, p_data.w});
    auto &rgbaDispatcher = rgbaWidget.addPlugin<DataDispatcher<Color>>();
    rgbaDispatcher.registerReference(reinterpret_cast<Color &>(p_data));
    rgbaWidget.enabled = false;
    rgbaWidget.lineBreak = false;
    
    auto &xyzwButton = rightSide.createWidget<ButtonSimple>("XYZW");
    xyzwButton.idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    xyzwButton.lineBreak = false;
    
    auto &rgbaButton = rightSide.createWidget<ButtonSimple>("RGBA");
    rgbaButton.idleBackgroundColor = {0.7f, 0.5f, 0.0f};
    
    xyzwButton.clickedEvent += [&] {
        xyzWidget.enabled = true;
        rgbaWidget.enabled = false;
    };
    
    rgbaButton.clickedEvent += [&] {
        xyzWidget.enabled = false;
        rgbaWidget.enabled = true;
    };
}
} // namespace

MaterialEditor::MaterialEditor(const std::string &p_title,
                               bool p_opened,
                               const PanelWindowSettings &p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings) {
    createHeaderButtons();
    createWidget<Separator>();
    createMaterialSelector();
    _settings = &createWidget<Group>();
    createShaderSelector();
    createMaterialSettings();
    createShaderSettings();
    
    _settings->enabled = false;
    _shaderSettings->enabled = false;
    
    _materialDroppedEvent += std::bind(&MaterialEditor::onMaterialDropped, this);
    _shaderDroppedEvent += std::bind(&MaterialEditor::onShaderDropped, this);
}


}
}
}
