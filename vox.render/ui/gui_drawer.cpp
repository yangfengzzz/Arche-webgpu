//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui_drawer.h"

#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/drags/drag_single_scalar.h"
#include "ui/widgets/drags/drag_multiple_scalars.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/layout/columns.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/buttons/button.h"
#include "ui/widgets/buttons/button_small.h"
#include "ui/plugins/drag_drop_target.h"

namespace vox {
namespace ui {
const Color GUIDrawer::TitleColor = {0.85f, 0.65f, 0.0f};
const Color GUIDrawer::ClearButtonColor = {0.5f, 0.0f, 0.0f};
const float GUIDrawer::_MIN_FLOAT = -999999999.f;
const float GUIDrawer::_MAX_FLOAT = +999999999.f;
//OvRendering::Resources::Texture* GUIDrawer::__EMPTY_TEXTURE = nullptr;

//void GUIDrawer::ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture) {
//    __EMPTY_TEXTURE = &p_emptyTexture;
//}

void GUIDrawer::createTitle(WidgetContainer &p_root, const std::string &p_name) {
    p_root.createWidget<TextColored>(p_name, TitleColor);
}

void GUIDrawer::drawBoolean(WidgetContainer &p_root, const std::string &p_name, bool &p_data) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<CheckBox>();
    auto &dispatcher = widget.addPlugin<DataDispatcher<bool>>();
    dispatcher.registerReference(reinterpret_cast<bool &>(p_data));
}

void GUIDrawer::drawVec2(WidgetContainer &p_root, const std::string &p_name,
                         Vector2F &p_data, float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 2>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 2>>>();
    dispatcher.registerReference(reinterpret_cast<std::array<float, 2> &>(p_data));
}

void GUIDrawer::drawVec3(WidgetContainer &p_root, const std::string &p_name,
                         Vector3F &p_data, float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 3>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 3>>>();
    dispatcher.registerReference(reinterpret_cast<std::array<float, 3> &>(p_data));
}

void GUIDrawer::drawVec4(WidgetContainer &p_root, const std::string &p_name,
                         Vector4F &p_data, float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 4>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.registerReference(reinterpret_cast<std::array<float, 4> &>(p_data));
}

void GUIDrawer::drawQuat(WidgetContainer &p_root, const std::string &p_name,
                         QuaternionF &p_data, float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 4>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.registerReference(reinterpret_cast<std::array<float, 4> &>(p_data));
}

void GUIDrawer::drawString(WidgetContainer &p_root, const std::string &p_name, std::string &p_data) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<InputText>("");
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::string>>();
    dispatcher.registerReference(p_data);
}

void GUIDrawer::drawColor(WidgetContainer &p_root, const std::string &p_name, Color &p_color, bool p_hasAlpha) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<ColorEdit>(p_hasAlpha);
    auto &dispatcher = widget.addPlugin<DataDispatcher<Color>>();
    dispatcher.registerReference(p_color);
}

//Text& GUIDrawer::drawMesh(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Model *& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.createWidget<Group>();
//
//    auto& widget = rightSide.createWidget<Text>(displayedText);
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MODEL)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//OvUI::Widgets::Visual::Image& GUIDrawer::drawTexture(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Texture *& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.createWidget<Group>();
//
//    auto& widget = rightSide.createWidget<OvUI::Widgets::Visual::Image>(p_data ? p_data->id : (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0), Vector2F{ 75, 75 });
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.textureID.id = resource->id;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::Button>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0);
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GUIDrawer::drawShader(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Shader *& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.createWidget<Group>();
//
//    auto& widget = rightSide.createWidget<Text>(displayedText);
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SHADER)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GUIDrawer::drawMaterial(WidgetContainer & p_root, const std::string & p_name, OvCore::Resources::Material *& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.createWidget<Group>();
//
//    auto& widget = rightSide.createWidget<Text>(displayedText);
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MATERIAL)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::MaterialManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GUIDrawer::drawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto & rightSide = p_root.createWidget<Group>();
//
//    auto & widget = rightSide.createWidget<Text>(displayedText);
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SOUND)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::SoundManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto & resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GUIDrawer::drawAsset(WidgetContainer& p_root, const std::string& p_name, std::string& p_data, Event<>* p_updateNotifier)
//{
//    createTitle(p_root, p_name);
//
//    const std::string displayedText = (p_data.empty() ? std::string("Empty") : p_data);
//    auto& rightSide = p_root.createWidget<Group>();
//
//    auto& widget = rightSide.createWidget<Text>(displayedText);
//
//    widget.addPlugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        p_data = p_receivedData.first;
//        widget.content = p_receivedData.first;
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.createWidget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = ClearButtonColor;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = "";
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}

void GUIDrawer::drawBoolean(WidgetContainer &p_root, const std::string &p_name,
                            std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<CheckBox>();
    auto &dispatcher = widget.addPlugin<DataDispatcher<bool>>();
    
    dispatcher.registerGatherer([p_gatherer]() {
        bool value = p_gatherer();
        return reinterpret_cast<bool &>(value);
    });
    
    dispatcher.registerProvider([p_provider](bool p_value) {
        p_provider(reinterpret_cast<bool &>(p_value));
    });
}

void GUIDrawer::drawVec2(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector2F(void)> p_gatherer, std::function<void(Vector2F)> p_provider,
                         float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 2>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 2>>>();
    
    dispatcher.registerGatherer([p_gatherer]() {
        Vector2F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 2> &>(value);
    });
    
    dispatcher.registerProvider([p_provider](std::array<float, 2> p_value) {
        p_provider(reinterpret_cast<Vector2F &>(p_value));
    });
}

void GUIDrawer::drawVec3(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector3F(void)> p_gatherer, std::function<void(Vector3F)> p_provider,
                         float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 3>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 3>>>();
    
    dispatcher.registerGatherer([p_gatherer]() {
        Vector3F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 3> &>(value);
    });
    
    dispatcher.registerProvider([p_provider](std::array<float, 3> p_value) {
        p_provider(reinterpret_cast<Vector3F &>(p_value));
    });
}

void GUIDrawer::drawVec4(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector4F(void)> p_gatherer, std::function<void(Vector4F)> p_provider,
                         float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 4>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 4>>>();
    
    dispatcher.registerGatherer([p_gatherer]() {
        Vector4F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });
    
    dispatcher.registerProvider([p_provider](std::array<float, 4> p_value) {
        p_provider(reinterpret_cast<Vector4F &>(p_value));
    });
}

void GUIDrawer::drawQuat(WidgetContainer &p_root, const std::string &p_name,
                         std::function<QuaternionF(void)> p_gatherer, std::function<void(QuaternionF)> p_provider,
                         float p_step, float p_min, float p_max) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<DragMultipleScalars<float, 4>>(getDataType<float>(), p_min, p_max, 0.f, p_step, "", getFormat<float>());
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::array<float, 4>>>();
    
    dispatcher.registerGatherer([p_gatherer]() {
        QuaternionF value = p_gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });
    
    dispatcher.registerProvider([&dispatcher, p_provider](std::array<float, 4> p_value) {
        p_provider(reinterpret_cast<QuaternionF &>(p_value).normalized());
    });
}

void GUIDrawer::drawDDString(WidgetContainer &p_root, const std::string &p_name,
                             std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider,
                             const std::string &p_identifier) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<InputText>("");
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::string>>();
    dispatcher.registerGatherer(p_gatherer);
    dispatcher.registerProvider(p_provider);
    
    auto &ddTarget = widget.addPlugin<DDTarget<std::pair<std::string, Group *>>>(p_identifier);
    ddTarget.dataReceivedEvent += [&widget, &dispatcher](std::pair<std::string, Group *> p_data) {
        widget.content = p_data.first;
        dispatcher.notifyChange();
    };
}

void GUIDrawer::drawString(WidgetContainer &p_root, const std::string &p_name,
                           std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<InputText>("");
    auto &dispatcher = widget.addPlugin<DataDispatcher<std::string>>();
    dispatcher.registerGatherer(p_gatherer);
    dispatcher.registerProvider(p_provider);
}

void GUIDrawer::drawColor(WidgetContainer &p_root, const std::string &p_name,
                          std::function<Color(void)> p_gatherer, std::function<void(Color)> p_provider, bool p_hasAlpha) {
    createTitle(p_root, p_name);
    auto &widget = p_root.createWidget<ColorEdit>(p_hasAlpha);
    auto &dispatcher = widget.addPlugin<DataDispatcher<Color>>();
    dispatcher.registerGatherer(p_gatherer);
    dispatcher.registerProvider(p_provider);
}


}
}
