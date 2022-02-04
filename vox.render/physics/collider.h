//
//  collider.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

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
