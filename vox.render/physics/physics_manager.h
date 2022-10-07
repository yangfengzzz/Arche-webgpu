//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/PhysicsSystem.h>

#include "vox.render/singleton.h"

namespace vox {
class PhysicsManager : public Singleton<PhysicsManager> {
public:
    static PhysicsManager &GetSingleton();

    static PhysicsManager *GetSingletonPtr();

    PhysicsManager();

    ~PhysicsManager();

private:
    JPH::PhysicsSystem physics_system;
};
template <>
inline PhysicsManager *Singleton<PhysicsManager>::ms_singleton{nullptr};
}  // namespace vox