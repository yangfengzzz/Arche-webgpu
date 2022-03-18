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

#include "clear_mesh_generator.h"
#include "cpu_node_editor.h"
#include "cpu_noise_layers_generator.h"
#include "gpu_noise_layer_generator.h"

namespace vox {
namespace editor {
class MeshGeneratorManager : public Singleton<MeshGeneratorManager> {
public:
    static MeshGeneratorManager &getSingleton(void);
    
    static MeshGeneratorManager *getSingletonPtr(void);
    
    MeshGeneratorManager();
    
    void generate();

    void generateSync();

    void showSettings();

    void generateForTerrain();

    void generateForCustomBase();

    void executeKernels();

    void executeCPUGenerators();

    void loadKernels();

    double time = 0;
    
    bool windowStat = true;
    
private:
    std::atomic<bool> *isRemeshing;
    std::unique_ptr<ClearMeshGenerator> clearMeshGen;
    std::vector<std::unique_ptr<CPUNoiseLayersGenerator>> cpuNoiseLayers;
    std::vector<std::unique_ptr<GPUNoiseLayerGenerator>> gpuNoiseLayers;
    std::vector<std::unique_ptr<CPUNodeEditor>> cpuNodeEditors;
};


}
template<> inline editor::MeshGeneratorManager* Singleton<editor::MeshGeneratorManager>::msSingleton{nullptr};
}
#endif /* mesh_generator_manager_hpp */
