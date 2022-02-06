//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef collider_hpp
#define collider_hpp

#include "physics.h"
#include "component.h"
#include "update_flag.h"
#include <vector>

namespace vox {
namespace physics {
class Collider : public Component {
public:
    Collider(Entity *entity);
    
    void addShape(const ColliderShapePtr &shape);
    
    void removeShape(const ColliderShapePtr &shape);
    
    void clearShapes();
    
    PxRigidActor *handle();
    
public:
    void _onUpdate();
    
    virtual void _onLateUpdate() {
    }
    
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _onDestroy() override;
    
protected:
    friend class PhysicsManager;
    
    ssize_t _index = -1;
    std::unique_ptr<UpdateFlag> _updateFlag;
    physx::PxRigidActor *_nativeActor;
    std::vector<ColliderShapePtr> _shapes;
};

}
}
#endif /* collider_hpp */
