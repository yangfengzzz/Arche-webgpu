//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef clear_mesh_generator_hpp
#define clear_mesh_generator_hpp

#include <stdio.h>

namespace vox {
namespace editor {
class ClearMeshGenerator {
public:
    virtual void generate();

    virtual void showSettings();

    bool uiActive = false;
    bool useGPU = false;
    bool useGPUForNormals = false;
    double time = 0;
};


}
}
#endif /* clear_mesh_generator_hpp */
