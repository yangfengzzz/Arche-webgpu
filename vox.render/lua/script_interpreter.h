//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sol/sol.hpp>
#include <vector>

#include "vox.render/behaviour.h"
#include "vox.render/singleton.h"

namespace vox {
/**
 * Handles script interpretation
 */
class ScriptInterpreter : public Singleton<ScriptInterpreter> {
public:
    static ScriptInterpreter &GetSingleton();

    static ScriptInterpreter *GetSingletonPtr();

    /**
     * Constructor
     */
    explicit ScriptInterpreter(std::string scriptRootFolder);

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
    void consider(Behaviour *toConsider);

    /**
     * Unconsider a behaviour
     */
    void unconsider(Behaviour *toUnconsider);

    /**
     * Refresh every scripts
     */
    void refreshAll();

    /**
     * Returns true if every scripts are OK
     */
    [[nodiscard]] bool isOk() const;

private:
    std::unique_ptr<sol::state> _luaState{nullptr};
    std::string _scriptRootFolder;
    std::vector<Behaviour *> _behaviours;
    bool _isOk{};
};

template <>
inline ScriptInterpreter *Singleton<ScriptInterpreter>::ms_singleton{nullptr};

}  // namespace vox