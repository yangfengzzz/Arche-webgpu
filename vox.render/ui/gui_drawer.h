//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gui_drawer_hpp
#define gui_drawer_hpp

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "color.h"

#include "ui/widgets/widget_container.h"
#include "ui/widgets/texts/text.h"
#include "ui/widgets/drags/drag_single_scalar.h"
#include "ui/widgets/drags/drag_multiple_scalars.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/visual/image.h"

namespace vox {
namespace ui {
/**
 * Provide some helpers to draw UI elements
 */
class GUIDrawer {
public:
    static const Color TitleColor;
    static const Color ClearButtonColor;
    
    static const float _MIN_FLOAT;
    static const float _MAX_FLOAT;
    
    /**
     * Defines the texture to use when there is no texture in a texture resource field
     * @param p_emptyTexture
     */
    // static void ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture);
    
    /**
     * Draw a title with the title color
     * @param p_root p_root
     * @param p_name p_name
     */
    static void createTitle(WidgetContainer &p_root, const std::string &p_name);
    
    template<typename T>
    static void drawScalar(WidgetContainer &p_root, const std::string &p_name, T &p_data,
                           float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
    
    static void drawBoolean(WidgetContainer &p_root, const std::string &p_name, bool &p_data);
    
    static void drawVec2(WidgetContainer &p_root, const std::string &p_name, Vector2F &p_data,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawVec3(WidgetContainer &p_root, const std::string &p_name, Vector3F &p_data,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawVec4(WidgetContainer &p_root, const std::string &p_name, Vector4F &p_data,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawQuat(WidgetContainer &p_root, const std::string &p_name, QuaternionF &p_data,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawString(WidgetContainer &p_root, const std::string &p_name, std::string &p_data);
    
    static void drawColor(WidgetContainer &p_root, const std::string &p_name, Color &p_color, bool p_hasAlpha = false);
    
    // static Text& drawMesh(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Model*& p_data, Event<>* p_updateNotifier = nullptr);
    // static Image& drawTexture(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Texture*& p_data, Event<>* p_updateNotifier = nullptr);
    // static Text& drawShader(WidgetContainer& p_root, const std::string& p_name, OvRendering::Resources::Shader*& p_data, Event<>* p_updateNotifier = nullptr);
    // static Text& drawMaterial(WidgetContainer& p_root, const std::string& p_name, OvCore::Resources::Material*& p_data, Event<>* p_updateNotifier = nullptr);
    // static Text& drawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, Event<>* p_updateNotifier = nullptr);
    static Text &drawAsset(WidgetContainer &p_root, const std::string &p_name, std::string &p_data, Event<> *p_updateNotifier = nullptr);
    
    template<typename T>
    static void drawScalar(WidgetContainer &p_root, const std::string &p_name,
                           std::function<T(void)> p_gatherer, std::function<void(T)> p_provider,
                           float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
    
    static void drawBoolean(WidgetContainer &p_root, const std::string &p_name,
                            std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider);
    
    static void drawVec2(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector2F(void)> p_gatherer, std::function<void(Vector2F)> p_provider,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawVec3(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector3F(void)> p_gatherer, std::function<void(Vector3F)> p_provider,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawVec4(WidgetContainer &p_root, const std::string &p_name,
                         std::function<Vector4F(void)> p_gatherer, std::function<void(Vector4F)> p_provider,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawQuat(WidgetContainer &p_root, const std::string &p_name,
                         std::function<QuaternionF(void)> p_gatherer, std::function<void(QuaternionF)> p_provider,
                         float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
    
    static void drawDDString(WidgetContainer &p_root, const std::string &p_name,
                             std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider,
                             const std::string &p_identifier);
    
    static void drawString(WidgetContainer &p_root, const std::string &p_name,
                           std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider);
    
    static void drawColor(WidgetContainer &p_root, const std::string &p_name,
                          std::function<Color(void)> p_gatherer, std::function<void(Color)> p_provider, bool p_hasAlpha = false);
    
    template<typename T>
    static ImGuiDataType_ getDataType();
    
    template<typename T>
    static std::string getFormat();
    
private:
    // static OvRendering::Resources::Texture* __EMPTY_TEXTURE;
};


}
}

#include "gui_drawer-inl.h"

#endif /* gui_drawer_hpp */
