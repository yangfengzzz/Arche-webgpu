//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sprite_debug_hpp
#define sprite_debug_hpp

#include "script.h"
#include "mesh/buffer_mesh.h"

namespace vox {
class SpriteDebug : public Script {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    SpriteDebug(Entity *entity);
    
    void onUpdate(float deltaTime) override;
    
private:
    Entity *_spotEntity{nullptr};
    std::shared_ptr<BufferMesh> _spotLightMesh{nullptr};
    
    Entity *_pointEntity{nullptr};
    std::shared_ptr<BufferMesh> _pointLightMesh{nullptr};
};
}

#endif /* sprite_debug_hpp */
