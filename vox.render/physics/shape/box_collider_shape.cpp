//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "box_collider_shape.h"
#include "../physics_manager.h"

#ifdef _DEBUG
#include "mesh/mesh_renderer.h"
#include "mesh/wireframe_primitive_mesh.h"
#include "scene.h"
#include "material/unlit_material.h"
#endif

namespace vox {
namespace physics {
BoxColliderShape::BoxColliderShape() : ColliderShape() {
    auto halfExtent = _half * _scale;
    _nativeGeometry = std::make_shared<PxBoxGeometry>(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape = PhysicsManager::_nativePhysics()->createShape(*_nativeGeometry, *_nativeMaterial, true);
    _nativeShape->setQueryFilterData(PxFilterData(PhysicsManager::_idGenerator++, 0, 0, 0));
    setLocalPose(_pose);
}

Vector3F BoxColliderShape::size() {
    return _half * 2.f;
}

void BoxColliderShape::setSize(const Vector3F &size) {
    _half = size * 0.5f;
    auto halfExtent = _half * _scale;
    static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents = PxVec3(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncBoxGeometry();
#endif
}

void BoxColliderShape::setWorldScale(const Vector3F &scale) {
    ColliderShape::setWorldScale(scale);
    
    _scale = scale;
    auto halfExtent = _half * _scale;
    static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents = PxVec3(halfExtent.x, halfExtent.y, halfExtent.z);
    _nativeShape->setGeometry(*_nativeGeometry);
    
#ifdef _DEBUG
    _syncBoxGeometry();
#endif
}

#ifdef _DEBUG
void BoxColliderShape::setEntity(EntityPtr value) {
    ColliderShape::setEntity(value);
    
    auto renderer = _entity->addComponent<MeshRenderer>();
    renderer->setMaterial(std::make_shared<UnlitMaterial>(value->scene()->device()));
    renderer->setMesh(WireframePrimitiveMesh::createCuboidWireFrame(value->scene()->device(), 1, 1, 1));
    _syncBoxGeometry();
}

void BoxColliderShape::_syncBoxGeometry() {
    if (_entity) {
        auto halfExtents = static_cast<PxBoxGeometry *>(_nativeGeometry.get())->halfExtents;
        _entity->transform->setScale(halfExtents.x * 2, halfExtents.y * 2, halfExtents.z * 2);
    }
}
#endif

}
}
