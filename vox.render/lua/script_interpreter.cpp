//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script_interpreter.h"
#include "lua_binder.h"

namespace vox {
ScriptInterpreter::ScriptInterpreter(const std::string& scriptRootFolder) :
_scriptRootFolder(scriptRootFolder) {
    createLuaContextAndBindGlobals();
    
    /* Listen to behaviours */
    Behaviour::createdEvent += std::bind(&ScriptInterpreter::consider, this, std::placeholders::_1);
    Behaviour::destroyedEvent += std::bind(&ScriptInterpreter::unconsider, this, std::placeholders::_1);
}

ScriptInterpreter::~ScriptInterpreter() {
    destroyLuaContext();
}




}
