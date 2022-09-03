//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/gui_drawer.h"

#include <utility>

#include "vox.render/ui/plugins/drag_drop_target.h"
#include "vox.render/ui/widgets/buttons/button_small.h"
#include "vox.render/ui/widgets/drags/drag_multiple_scalars.h"
#include "vox.render/ui/widgets/input_fields/input_text.h"
#include "vox.render/ui/widgets/layout/group.h"
#include "vox.render/ui/widgets/selection/check_box.h"
#include "vox.render/ui/widgets/selection/color_edit.h"
#include "vox.render/ui/widgets/texts/text_colored.h"

namespace vox::ui {
const Color GuiDrawer::title_color_ = {0.85f, 0.65f, 0.0f};
const Color GuiDrawer::clear_button_color_ = {0.5f, 0.0f, 0.0f};
const float GuiDrawer::min_float_ = -999999999.f;
const float GuiDrawer::max_float_ = +999999999.f;
// OvRendering::Resources::Texture* GuiDrawer::__EMPTY_TEXTURE = nullptr;

// void GuiDrawer::ProvideEmptyTexture(OvRendering::Resources::Texture& emptyTexture) {
//     __EMPTY_TEXTURE = &emptyTexture;
// }

void GuiDrawer::CreateTitle(WidgetContainer &root, const std::string &name) {
    root.CreateWidget<TextColored>(name, title_color_);
}

void GuiDrawer::DrawBoolean(WidgetContainer &root, const std::string &name, bool &data) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<CheckBox>();
    auto &dispatcher = widget.AddPlugin<DataDispatcher<bool>>();
    dispatcher.RegisterReference(reinterpret_cast<bool &>(data));
}

void GuiDrawer::DrawVec2(
        WidgetContainer &root, const std::string &name, Vector2F &data, float step, float min, float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();
    dispatcher.RegisterReference(reinterpret_cast<std::array<float, 2> &>(data));
}

void GuiDrawer::DrawVec3(
        WidgetContainer &root, const std::string &name, Vector3F &data, float step, float min, float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();
    dispatcher.RegisterReference(reinterpret_cast<std::array<float, 3> &>(data));
}

void GuiDrawer::DrawVec4(
        WidgetContainer &root, const std::string &name, Vector4F &data, float step, float min, float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4> &>(data));
}

void GuiDrawer::DrawQuat(
        WidgetContainer &root, const std::string &name, QuaternionF &data, float step, float min, float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.RegisterReference(reinterpret_cast<std::array<float, 4> &>(data));
}

void GuiDrawer::DrawString(WidgetContainer &root, const std::string &name, std::string &data) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<InputText>("");
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
    dispatcher.RegisterReference(data);
}

void GuiDrawer::DrawColor(WidgetContainer &root, const std::string &name, Color &color, bool has_alpha) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<ColorEdit>(has_alpha);
    auto &dispatcher = widget.AddPlugin<DataDispatcher<Color>>();
    dispatcher.RegisterReference(color);
}

// Text& GuiDrawer::drawMesh(WidgetContainer & root, const std::string & name, OvRendering::Resources::Model *& data,
// Event<>* updateNotifier)
//{
//     create_title(root, name);
//
//     std::string displayedText = (data ? data->path : std::string("Empty"));
//     auto& rightSide = root.create_widget<Group>();
//
//     auto& widget = rightSide.create_widget<Text>(displayedText);
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         if (OvTools::Utils::PathParser::GetFileType(receivedData.first) ==
//         OvTools::Utils::PathParser::EFileType::MODEL)
//         {
//             if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(receivedData.first);
//             resource)
//             {
//                 data = resource;
//                 widget.content = receivedData.first;
//                 if (updateNotifier)
//                     updateNotifier->Invoke();
//             }
//         }
//     };
//
//     widget.lineBreak = false;
//
//     auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = nullptr;
//         widget.content = "Empty";
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }
//
// OvUI::Widgets::Visual::Image& GuiDrawer::drawTexture(WidgetContainer & root, const std::string & name,
// OvRendering::Resources::Texture *& data, Event<>* updateNotifier)
//{
//     create_title(root, name);
//
//     std::string displayedText = (data ? data->path : std::string("Empty"));
//     auto& rightSide = root.create_widget<Group>();
//
//     auto& widget = rightSide.create_widget<OvUI::Widgets::Visual::Image>(data ? data->id : (__EMPTY_TEXTURE ?
//     __EMPTY_TEXTURE->id : 0), Vector2F{ 75, 75 });
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         if (OvTools::Utils::PathParser::GetFileType(receivedData.first) ==
//         OvTools::Utils::PathParser::EFileType::TEXTURE)
//         {
//             if (auto resource =
//             OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(receivedData.first); resource)
//             {
//                 data = resource;
//                 widget.textureID.id = resource->id;
//                 if (updateNotifier)
//                     updateNotifier->Invoke();
//             }
//         }
//     };
//
//     widget.lineBreak = false;
//
//     auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::Button>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = nullptr;
//         widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0);
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }
//
// Text& GuiDrawer::drawShader(WidgetContainer & root, const std::string & name, OvRendering::Resources::Shader *& data,
// Event<>* updateNotifier)
//{
//     create_title(root, name);
//
//     std::string displayedText = (data ? data->path : std::string("Empty"));
//     auto& rightSide = root.create_widget<Group>();
//
//     auto& widget = rightSide.create_widget<Text>(displayedText);
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         if (OvTools::Utils::PathParser::GetFileType(receivedData.first) ==
//         OvTools::Utils::PathParser::EFileType::SHADER)
//         {
//             if (auto resource = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(receivedData.first);
//             resource)
//             {
//                 data = resource;
//                 widget.content = receivedData.first;
//                 if (updateNotifier)
//                     updateNotifier->Invoke();
//             }
//         }
//     };
//
//     widget.lineBreak = false;
//
//     auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = nullptr;
//         widget.content = "Empty";
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }
//
// Text& GuiDrawer::drawMaterial(WidgetContainer & root, const std::string & name, OvCore::Resources::Material *& data,
// Event<>* updateNotifier)
//{
//     create_title(root, name);
//
//     std::string displayedText = (data ? data->path : std::string("Empty"));
//     auto& rightSide = root.create_widget<Group>();
//
//     auto& widget = rightSide.create_widget<Text>(displayedText);
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         if (OvTools::Utils::PathParser::GetFileType(receivedData.first) ==
//         OvTools::Utils::PathParser::EFileType::MATERIAL)
//         {
//             if (auto resource =
//             OVSERVICE(OvCore::ResourceManagement::MaterialManager).GetResource(receivedData.first); resource)
//             {
//                 data = resource;
//                 widget.content = receivedData.first;
//                 if (updateNotifier)
//                     updateNotifier->Invoke();
//             }
//         }
//     };
//
//     widget.lineBreak = false;
//
//     auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = nullptr;
//         widget.content = "Empty";
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }
//
// Text& GuiDrawer::drawSound(WidgetContainer& root, const std::string& name, OvAudio::Resources::Sound*& data, Event<>*
// updateNotifier)
//{
//     create_title(root, name);
//
//     std::string displayedText = (data ? data->path : std::string("Empty"));
//     auto & rightSide = root.create_widget<Group>();
//
//     auto & widget = rightSide.create_widget<Text>(displayedText);
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         if (OvTools::Utils::PathParser::GetFileType(receivedData.first) ==
//         OvTools::Utils::PathParser::EFileType::SOUND)
//         {
//             if (auto resource = OVSERVICE(OvCore::ResourceManagement::SoundManager).GetResource(receivedData.first);
//             resource)
//             {
//                 data = resource;
//                 widget.content = receivedData.first;
//                 if (updateNotifier)
//                     updateNotifier->Invoke();
//             }
//         }
//     };
//
//     widget.lineBreak = false;
//
//     auto & resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = nullptr;
//         widget.content = "Empty";
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }
//
// Text& GuiDrawer::draw_asset(WidgetContainer& root, const std::string& name, std::string& data, Event<>*
// updateNotifier)
//{
//     create_title(root, name);
//
//     const std::string displayedText = (data.empty() ? std::string("Empty") : data);
//     auto& rightSide = root.create_widget<Group>();
//
//     auto& widget = rightSide.create_widget<Text>(displayedText);
//
//     widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &data,
//     updateNotifier](auto receivedData)
//     {
//         data = receivedData.first;
//         widget.content = receivedData.first;
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     widget.lineBreak = false;
//
//     auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//     resetButton.idleBackgroundColor = clear_button_color_;
//     resetButton.ClickedEvent += [&widget, &data, updateNotifier]
//     {
//         data = "";
//         widget.content = "Empty";
//         if (updateNotifier)
//             updateNotifier->Invoke();
//     };
//
//     return widget;
// }

void GuiDrawer::DrawBoolean(WidgetContainer &root,
                            const std::string &name,
                            const std::function<bool(void)> &gatherer,
                            const std::function<void(bool)> &provider) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<CheckBox>();
    auto &dispatcher = widget.AddPlugin<DataDispatcher<bool>>();

    dispatcher.RegisterGatherer([gatherer]() {
        bool value = gatherer();
        return reinterpret_cast<bool &>(value);
    });

    dispatcher.RegisterProvider([provider](bool value) { provider(reinterpret_cast<bool &>(value)); });
}

void GuiDrawer::DrawVec2(WidgetContainer &root,
                         const std::string &name,
                         const std::function<Vector2F(void)> &gatherer,
                         const std::function<void(Vector2F)> &provider,
                         float step,
                         float min,
                         float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 2>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 2>>>();

    dispatcher.RegisterGatherer([gatherer]() {
        Vector2F value = gatherer();
        return reinterpret_cast<const std::array<float, 2> &>(value);
    });

    dispatcher.RegisterProvider(
            [provider](std::array<float, 2> value) { provider(reinterpret_cast<Vector2F &>(value)); });
}

void GuiDrawer::DrawVec3(WidgetContainer &root,
                         const std::string &name,
                         const std::function<Vector3F(void)> &gatherer,
                         const std::function<void(Vector3F)> &provider,
                         float step,
                         float min,
                         float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 3>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 3>>>();

    dispatcher.RegisterGatherer([gatherer]() {
        Vector3F value = gatherer();
        return reinterpret_cast<const std::array<float, 3> &>(value);
    });

    dispatcher.RegisterProvider(
            [provider](std::array<float, 3> value) { provider(reinterpret_cast<Vector3F &>(value)); });
}

void GuiDrawer::DrawVec4(WidgetContainer &root,
                         const std::string &name,
                         const std::function<Vector4F(void)> &gatherer,
                         const std::function<void(Vector4F)> &provider,
                         float step,
                         float min,
                         float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();

    dispatcher.RegisterGatherer([gatherer]() {
        Vector4F value = gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });

    dispatcher.RegisterProvider(
            [provider](std::array<float, 4> value) { provider(reinterpret_cast<Vector4F &>(value)); });
}

void GuiDrawer::DrawQuat(WidgetContainer &root,
                         const std::string &name,
                         const std::function<QuaternionF(void)> &gatherer,
                         const std::function<void(QuaternionF)> &provider,
                         float step,
                         float min,
                         float max) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragMultipleScalars<float, 4>>(GetDataType<float>(), min, max, 0.f, step, "",
                                                                    GetFormat<float>());
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::array<float, 4>>>();

    dispatcher.RegisterGatherer([gatherer]() {
        QuaternionF value = gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });

    dispatcher.RegisterProvider(
            [provider](std::array<float, 4> value) { provider(reinterpret_cast<QuaternionF &>(value).normalized()); });
}

void GuiDrawer::DrawDdstring(WidgetContainer &root,
                             const std::string &name,
                             std::function<std::string()> gatherer,
                             std::function<void(std::string)> provider,
                             const std::string &identifier) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<InputText>("");
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
    dispatcher.RegisterGatherer(std::move(gatherer));
    dispatcher.RegisterProvider(std::move(provider));

    auto &dd_target = widget.AddPlugin<DDTarget<std::pair<std::string, Group *>>>(identifier);
    dd_target.data_received_event_ += [&widget, &dispatcher](const std::pair<std::string, Group *> &data) {
        widget.content_ = data.first;
        dispatcher.NotifyChange();
    };
}

void GuiDrawer::DrawString(WidgetContainer &root,
                           const std::string &name,
                           std::function<std::string(void)> gatherer,
                           std::function<void(std::string)> provider) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<InputText>("");
    auto &dispatcher = widget.AddPlugin<DataDispatcher<std::string>>();
    dispatcher.RegisterGatherer(std::move(gatherer));
    dispatcher.RegisterProvider(std::move(provider));
}

void GuiDrawer::DrawColor(WidgetContainer &root,
                          const std::string &name,
                          std::function<Color(void)> gatherer,
                          std::function<void(Color)> provider,
                          bool has_alpha) {
    CreateTitle(root, name);
    auto &widget = root.CreateWidget<ColorEdit>(has_alpha);
    auto &dispatcher = widget.AddPlugin<DataDispatcher<Color>>();
    dispatcher.RegisterGatherer(std::move(gatherer));
    dispatcher.RegisterProvider(std::move(provider));
}

}  // namespace vox::ui
