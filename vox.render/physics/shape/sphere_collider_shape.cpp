//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sphere_collider_shape.h"
#include "../physics_manager.h"

#ifdef _DEBUG
#include "mesh/mesh_renderer.h"
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#include "material/unlit_material.h"
#endif

namespace vox {
namespace physics {
SphereColliderShape::SphereColliderShape() {
    _nativeGeometry = std::make_shared<PxSphereGeometry>(_radius * std::max(std::max(_scale.x, _scale.y), _scale.z));
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    setLocalPose(_pose);
}

float SphereColliderShape::radius() {
    return _radius;
}

void SphereColliderShape::setRadius(float value) {
    _radius = value;
    static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius = value * std::max(std::max(_scale.x, _scale.y), _scale.z);
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncSphereGeometry();
#endif
}

void SphereColliderShape::setWorldScale(const Vector3F &scale) {
    ColliderShape::setWorldScale(scale);
    
    _scale = scale;
    static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius = _radius * std::max(std::max(_scale.x, _scale.y), _scale.z);
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncSphereGeometry();
#endif
}

#ifdef _DEBUG
void SphereColliderShape::setEntity(Entity* value) {
    ColliderShape::setEntity(value);
    
    auto renderer = _entity->addComponent<MeshRenderer>();
    renderer->setMaterial(std::make_shared<UnlitMaterial>(value->scene()->device()));
    renderer->setMesh(WireframePrimitiveMesh::createSphereWireFrame(value->scene()->device(), 1));
    _syncSphereGeometry();
}

void SphereColliderShape::_syncSphereGeometry() {
    if (_entity) {
        auto radius = static_cast<PxSphereGeometry *>(_nativeGeometry.get())->radius;
        _entity->transform->setScale(radius, radius, radius);
    }
}
#endif

}
}
