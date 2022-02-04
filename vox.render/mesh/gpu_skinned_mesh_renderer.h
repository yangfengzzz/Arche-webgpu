//
//  gpu_skinned_mesh_renderer.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/16.
//

#ifndef gpu_skinned_mesh_renderer_hpp
#define gpu_skinned_mesh_renderer_hpp

#include "mesh_renderer.h"
#include "core/cpp_mtl.h"

namespace vox {

class GPUSkinnedMeshRenderer : public MeshRenderer {
public:
    struct Skin {
        std::string name;
        std::vector<Matrix4x4F> inverseBindMatrices;
        std::vector<Entity *> joints;
    };
    using SkinPtr = std::shared_ptr<Skin>;
    
public:
    GPUSkinnedMeshRenderer(Entity *entity);
    
    /**
     * Skin Object.
     */
    SkinPtr skin();
    
    void setSkin(const SkinPtr &skin);
    
    void update(float deltaTime) override;
    
private:
    SkinPtr _skin;
    
    void _initJoints();
    
    bool _hasInitJoints = false;
    
    std::shared_ptr<MTL::Buffer> matrixPalette;
    std::vector<float> jointMatrix{};
    ShaderProperty _jointMatrixProperty;
};

}

#endif /* gpu_skinned_mesh_renderer_hpp */
