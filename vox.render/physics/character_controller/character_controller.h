//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef CharacterController_hpp
#define CharacterController_hpp

#include "../physics.h"
#include "../../component.h"
#include "point3.h"

namespace vox {
namespace physics {

class CharacterController : public Component {
public:
    CharacterController(Entity *entity);
    
    PxControllerCollisionFlags move(const Vector3F &disp, float minDist, float elapsedTime);
    
    bool setPosition(const Point3F &position);
    
    Point3F position() const;
    
    bool setFootPosition(const Vector3F &position);
    
    Vector3F footPosition() const;
    
    void setStepOffset(const float offset);
    
    float stepOffset() const;
    
    void setNonWalkableMode(PxControllerNonWalkableMode::Enum flag);
    
    PxControllerNonWalkableMode::Enum nonWalkableMode() const;
    
    float contactOffset() const;
    
    void setContactOffset(float offset);
    
    Vector3F upDirection() const;
    
    void setUpDirection(const Vector3F &up);
    
    float slopeLimit() const;
    
    void setSlopeLimit(float slopeLimit);
    
    void invalidateCache();
    
    void state(PxControllerState &state) const;
    
    void stats(PxControllerStats &stats) const;
    
    void resize(float height);
    
private:
    friend class PhysicsManager;
    
    void _onLateUpdate();
    
    void _onEnable() override;
    
    void _onDisable() override;
    
protected:
    PxController *_nativeController;
};

}
}

#endif /* CharacterController_hpp */
