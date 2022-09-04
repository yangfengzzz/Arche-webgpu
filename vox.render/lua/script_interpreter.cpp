//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lua/script_interpreter.h"

#include <utility>

#include "vox.render/lua/lua_binder.h"

namespace vox {
ScriptInterpreter::ScriptInterpreter(std::string scriptRootFolder) : _scriptRootFolder(std::move(scriptRootFolder)) {
    createLuaContextAndBindGlobals();

    /* Listen to behaviours */
    Behaviour::createdEvent += std::bind(&ScriptInterpreter::consider, this, std::placeholders::_1);
    Behaviour::destroyedEvent += std::bind(&ScriptInterpreter::unconsider, this, std::placeholders::_1);
}

ScriptInterpreter::~ScriptInterpreter() { destroyLuaContext(); }

void ScriptInterpreter::createLuaContextAndBindGlobals() {
    if (!_luaState) {
        _luaState = std::make_unique<sol::state>();
        _luaState->open_libraries(sol::lib::base, sol::lib::math);
        LuaBinder::callBinders(*_luaState);
        _isOk = true;

        std::for_each(_behaviours.begin(), _behaviours.end(), [this](Behaviour *behaviour) {
            if (!behaviour->registerToLuaContext(*_luaState, _scriptRootFolder)) _isOk = false;
        });

        if (!_isOk) LOGE("Script interpreter failed to register scripts. Check your lua scripts")
    }
}

void ScriptInterpreter::destroyLuaContext() {
    if (_luaState) {
        std::for_each(_behaviours.begin(), _behaviours.end(),
                      [](Behaviour *behaviour) { behaviour->unregisterFromLuaContext(); });

        _luaState.reset();
        _isOk = false;
    }
}

void ScriptInterpreter::consider(Behaviour *p_toConsider) {
    if (_luaState) {
        _behaviours.push_back(p_toConsider);

        if (!p_toConsider->registerToLuaContext(*_luaState, _scriptRootFolder)) _isOk = false;
    }
}

void ScriptInterpreter::unconsider(Behaviour *p_toUnconsider) {
    if (_luaState) p_toUnconsider->unregisterFromLuaContext();

    _behaviours.erase(std::remove_if(_behaviours.begin(), _behaviours.end(),
                                     [p_toUnconsider](Behaviour *behaviour) { return p_toUnconsider == behaviour; }),
                      _behaviours.end());

    refreshAll();  // Unconsidering a script is impossible with Lua, we have to reparse every behaviour
}

void ScriptInterpreter::refreshAll() {
    destroyLuaContext();
    createLuaContextAndBindGlobals();
}

bool ScriptInterpreter::isOk() const { return _isOk; }

}  // namespace vox
