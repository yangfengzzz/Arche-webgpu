//
//  physics.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

#include "physics.h"

namespace vox {
namespace physics {
Physics::Physics() {
    PxFoundation *gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), false, nullptr);
}

PxPhysics *Physics::operator()() {
    return _physics;
}


}
}
