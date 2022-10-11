//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/material_editor.h"

#include "vox.render/ui/gui_drawer.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/visual/separator.h"

namespace vox::editor::ui {
namespace {
void drawHybridVec3(
        WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data, float p_step, float p_min, float p_max) {
    GuiDrawer::CreateTitle(p_root, p_name);

    auto &rightSide = p_root.CreateWidget<Group>();

    auto &xyzWidget = rightSide.CreateWidget<DragMultipleScalars<float, 3>>(
            GuiDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GuiDrawer::GetFormat<float>());
    auto &xyzDispatcher = xyzWidget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
    xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 3> &>(p_data));
    xyzWidget.line_break_ = false;

    auto &rgbWidget = rightSide.CreateWidget<ColorEdit>(false, Color{p_data.x, p_data.y, p_data.z});
    auto &rgbDispatcher = rgbWidget.AddPlugin<DataDispatcher<Color>>();
    rgbDispatcher.RegisterReference(reinterpret_cast<Color &>(p_data));
    rgbWidget.enabled_ = false;
    rgbWidget.line_break_ = false;

    auto &xyzButton = rightSide.CreateWidget<ButtonSimple>("XYZ");
    xyzButton.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyzButton.line_break_ = false;

    auto &rgbButton = rightSide.CreateWidget<ButtonSimple>("RGB");
    rgbButton.idle_background_color_ = {0.7f, 0.5f, 0.0f};

    xyzButton.clicked_event_ += [&] {
        xyzWidget.enabled_ = true;
        rgbWidget.enabled_ = false;
    };

    rgbButton.clicked_event_ += [&] {
        xyzWidget.enabled_ = false;
        rgbWidget.enabled_ = true;
    };
}

void drawHybridVec4(
        WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data, float p_step, float p_min, float p_max) {
    GuiDrawer::CreateTitle(p_root, p_name);

    auto &rightSide = p_root.CreateWidget<Group>();

    auto &xyzWidget = rightSide.CreateWidget<DragMultipleScalars<float, 4>>(
            GuiDrawer::GetDataType<float>(), p_min, p_max, 0.f, p_step, "", GuiDrawer::GetFormat<float>());
    auto &xyzDispatcher = xyzWidget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
    xyzDispatcher.RegisterReference(reinterpret_cast<std::array<float, 4> &>(p_data));
    xyzWidget.line_break_ = false;

    auto &rgbaWidget = rightSide.CreateWidget<ColorEdit>(true, Color{p_data.x, p_data.y, p_data.z, p_data.w});
    auto &rgbaDispatcher = rgbaWidget.AddPlugin<DataDispatcher<Color>>();
    rgbaDispatcher.RegisterReference(reinterpret_cast<Color &>(p_data));
    rgbaWidget.enabled_ = false;
    rgbaWidget.line_break_ = false;

    auto &xyzwButton = rightSide.CreateWidget<ButtonSimple>("XYZW");
    xyzwButton.idle_background_color_ = {0.7f, 0.5f, 0.0f};
    xyzwButton.line_break_ = false;

    auto &rgbaButton = rightSide.CreateWidget<ButtonSimple>("RGBA");
    rgbaButton.idle_background_color_ = {0.7f, 0.5f, 0.0f};

    xyzwButton.clicked_event_ += [&] {
        xyzWidget.enabled_ = true;
        rgbaWidget.enabled_ = false;
    };

    rgbaButton.clicked_event_ += [&] {
        xyzWidget.enabled_ = false;
        rgbaWidget.enabled_ = true;
    };
}
}  // namespace

MaterialEditor::MaterialEditor(const std::string &p_title, bool p_opened, const PanelWindowSettings &p_windowSettings)
    : PanelWindow(p_title, p_opened, p_windowSettings) {
    createHeaderButtons();
    CreateWidget<Separator>();
    createMaterialSelector();
    _settings = &CreateWidget<Group>();
    createShaderSelector();
    createMaterialSettings();
    createShaderSettings();

    _settings->enabled_ = false;
    _shaderSettings->enabled_ = false;

    _materialDroppedEvent += std::bind(&MaterialEditor::onMaterialDropped, this);
    _shaderDroppedEvent += std::bind(&MaterialEditor::onShaderDropped, this);
}

void MaterialEditor::refresh() {}

void MaterialEditor::setTarget(const std::shared_ptr<Material> &new_target) {}

std::shared_ptr<Material> MaterialEditor::getTarget() const { return nullptr; }

void MaterialEditor::removeTarget() {}

void MaterialEditor::preview() {}

void MaterialEditor::reset() {}

void MaterialEditor::onMaterialDropped() {}

void MaterialEditor::onShaderDropped() {}

void MaterialEditor::createHeaderButtons() {}

void MaterialEditor::createMaterialSelector() {}

void MaterialEditor::createShaderSelector() {}

void MaterialEditor::createMaterialSettings() {}

void MaterialEditor::createShaderSettings() {}

void MaterialEditor::generateShaderSettingsContent() {}

void MaterialEditor::generateMaterialSettingsContent() {}

}  // namespace vox::editor::ui
