//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_map>

#include "vox.editor/ui/menu_bar.h"
#include "vox.render/ui/canvas.h"
#include "vox.render/ui/widgets/panel.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Handle the creation and storage of UI panels
 */
class PanelsManager {
public:
    /**
     * Constructor
     * @param p_canvas p_canvas
     */
    explicit PanelsManager(Canvas& p_canvas);

    /**
     * Create a panel
     * @param p_id p_id
     * @param p_args p_args
     */
    template <typename T, typename... Args>
    void createPanel(const std::string& p_id, Args&&... p_args) {
        if constexpr (std::is_base_of<PanelWindow, T>::value) {
            _panels.emplace(p_id, std::make_unique<T>(p_id, std::forward<Args>(p_args)...));
            T& instance = *static_cast<T*>(_panels.at(p_id).get());
            getPanelAs<MenuBar>("Menu Bar").registerPanel(instance.name, instance);
        } else {
            _panels.emplace(p_id, std::make_unique<T>(std::forward<Args>(p_args)...));
        }
        _canvas.AddPanel(*_panels.at(p_id));
    }

    /**
     * Returns the panel identified by the given id casted in the given type
     * @param p_id p_id
     */
    template <typename T>
    T& getPanelAs(const std::string& p_id) {
        return *static_cast<T*>(_panels[p_id].get());
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Panel>> _panels;
    Canvas& _canvas;
};

}  // namespace editor::ui
}  // namespace vox