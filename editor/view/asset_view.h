//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef asset_view_hpp
#define asset_view_hpp

#include <variant>
#include "view_controllable.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
/**
 * Provide a view for assets
 */
class AssetView : public ViewControllable {
public:
    //    using ViewableResource = std::variant<OvRendering::Resources::Model*, OvRendering::Resources::Texture*, OvCore::Resources::Material*>;
    
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    AssetView(const std::string &p_title,
              bool p_opened,
              const PanelWindowSettings &p_windowSettings);
    
    /**
     * Custom implementation of the render method
     */
    virtual void _render_Impl() override;
    
    //    /**
    //     * Defines the resource to preview
    //     * @parma p_resource
    //     */
    //    void setResource(ViewableResource p_resource);
    //
    //    /**
    //     * Return the currently previewed resource
    //     */
    //    ViewableResource resource() const;
    
private:
    //    ViewableResource _resource;
};

}
}
}
#endif /* asset_view_hpp */
