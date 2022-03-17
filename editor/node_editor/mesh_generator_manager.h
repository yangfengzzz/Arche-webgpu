//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_generator_manager_hpp
#define mesh_generator_manager_hpp

#include <vector>
#include <atomic>
#include "singleton.h"

namespace vox {
namespace editor {
class MeshGeneratorManager : public Singleton<MeshGeneratorManager> {
public:
    static MeshGeneratorManager &getSingleton(void);
    
    static MeshGeneratorManager *getSingletonPtr(void);
};


}
template<> inline editor::MeshGeneratorManager* Singleton<editor::MeshGeneratorManager>::msSingleton{nullptr};
}
#endif /* mesh_generator_manager_hpp */
