//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh_generator_manager.h"

namespace vox {
editor::MeshGeneratorManager *editor::MeshGeneratorManager::getSingletonPtr(void) {
    return msSingleton;
}

editor::MeshGeneratorManager &editor::MeshGeneratorManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}
namespace editor {

}
}
