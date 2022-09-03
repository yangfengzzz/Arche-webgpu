//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "behaviour.h"

#include "entity.h"
#include "ui/widgets/texts/text_colored.h"
#include "vox.base/logging.h"

namespace vox {
std::string Behaviour::name() { return "Behaviour"; }

Behaviour::Behaviour(Entity *entity) : Script(entity) { createdEvent.Invoke(this); }

Behaviour::~Behaviour() { destroyedEvent.Invoke(this); }

bool Behaviour::registerToLuaContext(sol::state &p_luaState, const std::string &p_scriptFolder) {
    auto result = p_luaState.safe_script_file(p_scriptFolder + scriptName + ".lua", &sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error err = result;
        LOGE(err.what())
        return false;
    } else {
        if (result.return_count() == 1 && result[0].is<sol::table>()) {
            _object = result[0];
            _object["owner"] = _entity;
            return true;
        } else {
            LOGE("{}.lua missing return expression", scriptName)
            return false;
        }
    }
}

void Behaviour::unregisterFromLuaContext() { _object = sol::lua_nil; }

sol::table &Behaviour::table() { return _object; }

// MARK: - Lua Call

void Behaviour::onAwake() { luaCall("onAwake"); }

void Behaviour::onEnable() { luaCall("onEnable"); }

void Behaviour::onDisable() { luaCall("onDisable"); }

void Behaviour::onDestroy() { luaCall("onDestroy"); }

void Behaviour::onStart() { luaCall("onStart"); }

void Behaviour::onUpdate(float deltaTime) { luaCall("onUpdate", deltaTime); }

void Behaviour::onLateUpdate(float deltaTime) { luaCall("onLateUpdate", deltaTime); }

void Behaviour::onBeginRender(Camera *camera) {}

void Behaviour::onEndRender(Camera *camera) {}

void Behaviour::inputEvent(const InputEvent &inputEvent) {}

void Behaviour::resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {}

void Behaviour::onInspector(ui::WidgetContainer &root) {
    if (_object.valid()) {
        root.CreateWidget<ui::TextColored>("Ready", Color::green);
        root.CreateWidget<ui::TextColored>("Your script gets interpreted by the engine with success", Color::white);
    } else {
        root.CreateWidget<ui::TextColored>("Compilation failed!", Color::red);
        root.CreateWidget<ui::TextColored>("Check the console for more information", Color::white);
    }
}

}  // namespace vox
