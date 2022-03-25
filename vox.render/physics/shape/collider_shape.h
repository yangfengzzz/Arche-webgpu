//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef collider_shape_hpp
#define collider_shape_hpp

#include "scene_forward.h"
#include "../physics.h"
#include "transform3.h"
#include <vector>

namespace vox {
namespace physics {
class ColliderShape {
public:
    ColliderShape();
    
    Collider *collider();
    
public:
    void setLocalPose(const Transform3F &pose);
    
    Transform3F localPose() const;
    
    void setPosition(const Vector3F &pos);
    
    Vector3F position() const;
    
    virtual void setWorldScale(const Vector3F &scale);
    
public:
    void setMaterial(PxMaterial *materials);
    
    PxMaterial *material();
    
public:
    void setQueryFilterData(const PxFilterData &data);
    
    PxFilterData queryFilterData();
    
    uint32_t uniqueID();
    
public:
    void setFlag(PxShapeFlag::Enum flag, bool value);
    
    void setFlags(PxShapeFlags inFlags);
    
    PxShapeFlags getFlags() const;
    
    bool trigger();
    
    void setTrigger(bool isTrigger);
    
    bool sceneQuery();
    
    void setSceneQuery(bool isQuery);
    
public:
#ifdef _DEBUG
    virtual void setEntity(Entity* value);

    void removeEntity(Entity* value);
    
    Point3F getLocalTranslation();
#endif
    
protected:
    friend class Collider;
    
    PxShape *_nativeShape = nullptr;
    std::shared_ptr<PxGeometry> _nativeGeometry = nullptr;
    PxMaterial *_nativeMaterial = nullptr;
    
    Collider *_collider = nullptr;
    
    Vector3F _scale = Vector3F(1, 1, 1);
    Transform3F _pose;
    static constexpr float halfSqrt = 0.70710678118655;
    
#ifdef _DEBUG
    Entity* _entity{nullptr};
#endif
};

}
}
#endif /* collider_shape_hpp */
