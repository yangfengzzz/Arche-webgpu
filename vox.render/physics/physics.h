//
//  physics.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

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
