//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef physics_hpp
#define physics_hpp

#include "vector3.h"
#include <PxPhysicsAPI.h>
#include <memory>

namespace vox {
namespace physics {
//MARK: - Forward Declare
class PhysicsMaterial;

class ColliderShape;

using ColliderShapePtr = std::shared_ptr<ColliderShape>;

class Collider;

class CharacterController;

class BoxCharacterController;

class CapsuleCharacterController;

using namespace physx;

//MARK: - PxPhysics
class Physics {
public:
    Physics();
    
    PxPhysics *operator()();
    
private:
    PxPhysics *_physics;
    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;
};

}
}

#endif /* physics_hpp */
