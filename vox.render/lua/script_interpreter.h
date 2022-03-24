//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef script_interpreter_hpp
#define script_interpreter_hpp

#include <vector>
#include <sol/sol.hpp>

#include "behaviour.h"

namespace vox {
/**
 * Handles script interpretation
 */
class ScriptInterpreter {
public:
    /**
     * Constructor
     */
    ScriptInterpreter(const std::string& scriptRootFolder);
    
    /**
     * Destructor
     */
    ~ScriptInterpreter();
    
    /**
     * Handle the creation of the lua context and bind engine functions to lua
     */
    void createLuaContextAndBindGlobals();
    
    /**
     * Destroy the lua context
     */
    void destroyLuaContext();
    
    /**
     * Consider a behaviour
     */
    void consider(Behaviour* toConsider);
    
    /**
     * Unconsider a behaviour
     */
    void unconsider(Behaviour* toUnconsider);
    
    /**
     * Refresh every scripts
     */
    void refreshAll();
    
    /**
     * Returns true if every scripts are OK
     */
    bool isOk() const;
    
private:
    std::unique_ptr<sol::state> _luaState{nullptr};
    std::string _scriptRootFolder;
    std::vector<Behaviour*> _behaviours;
    bool _isOk;
};

}
#endif /* script_interpreter_hpp */
