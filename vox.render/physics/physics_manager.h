//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef physics_manager_hpp
#define physics_manager_hpp

#include "physics.h"
#include <unordered_map>
#include <vector>
#include "ray3.h"
#include "hit_result.h"
#include "../layer.h"

namespace vox {
namespace physics {
/**
 * A physics manager is a collection of bodies and constraints which can interact.
 */
class PhysicsManager {
public:
    static uint32_t _idGenerator;
    static Physics _nativePhysics;
    
    PhysicsManager();
    
public:
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray);
    
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param outHitResult - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, HitResult &outHitResult);
    
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance);
    
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param outHitResult - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance, HitResult &outHitResult);
    
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param layerMask - Layer mask that is used to selectively ignore Colliders when casting
     * @returns Returns True if the ray intersects with a collider, otherwise false
     */
    bool raycast(const Ray3F &ray, float distance, Layer layerMask);
    
    /**
     * Casts a ray through the Scene and returns the first hit.
     * @param ray - The ray
     * @param distance - The max distance the ray should check
     * @param layerMask - Layer mask that is used to selectively ignore Colliders when casting
     * @param outHitResult - If true is returned, outHitResult will contain more detailed collision information
     * @returns Returns True if the ray intersects with a collider, otherwise false.
     */
    bool raycast(const Ray3F &ray, float distance, Layer layerMask, HitResult &outHitResult);
    
public:
    /**
     * Call on every frame to update pose of objects.
     */
    void update(float deltaTime);
    
    void callColliderOnUpdate();
    
    void callColliderOnLateUpdate();
    
    void callCharacterControllerOnLateUpdate();
    
private:
    friend class Collider;
    
    friend class CharacterController;
    
    friend class BoxCharacterController;
    
    friend class CapsuleCharacterController;
    
    /**
     * Add ColliderShape into the manager.
     * @param colliderShape - The Collider Shape.
     */
    void _addColliderShape(const ColliderShapePtr &colliderShape);
    
    /**
     * Remove ColliderShape.
     * @param colliderShape - The Collider Shape.
     */
    void _removeColliderShape(const ColliderShapePtr &colliderShape);
    
    /**
     * Add collider into the manager.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void _addCollider(Collider *collider);
    
    /**
     * Remove collider.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void _removeCollider(Collider *collider);
    
    /**
     * Add CharacterController into the manager.
     * @param characterController The Character Controller.
     */
    void _addCharacterController(CharacterController *characterController);
    
    /**
     * Remove CharacterController.
     * @param characterController The Character Controller.
     */
    void _removeCharacterController(CharacterController *characterController);
    
    bool _raycast(const Ray3F &ray, float distance,
                  std::function<void(uint32_t, float,
                                     const Vector3F &,
                                     const Point3F &)> outHitResult);
    
private:
    PxControllerManager *_nativeCharacterControllerManager;
    PxScene *_nativePhysicsManager;
    
    std::unordered_map<uint32_t, ColliderShapePtr> _physicalObjectsMap;
    std::vector<Collider *> _colliders;
    std::vector<CharacterController *> _controllers;
    
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactEnter;
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactExit;
    std::function<void(PxShape *obj1, PxShape *obj2)> onContactStay;
    
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerEnter;
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerExit;
    std::function<void(PxShape *obj1, PxShape *obj2)> onTriggerStay;
};

}
}

#endif /* physics_manager_hpp */
