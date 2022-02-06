//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef grid_hpp
#define grid_hpp

#include "script.h"

namespace vox {
namespace editor {
class Grid : public Script {
public:
    Grid(Entity *entity);
    
private:
    ModelMeshPtr createPlane(wgpu::Device& device);
    
private:
    MeshRenderer *_renderer;
    MaterialPtr _mtl;
};

}
}

#endif /* grid_hpp */
