//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gltf_loader_hpp
#define gltf_loader_hpp

#include "mesh/gpu_skinned_mesh_renderer.h"
#include "entity.h"
#include "texture_loader.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

namespace vox {
namespace loader {

class GLTFLoader {
public:
    std::vector<MTL::TexturePtr> textures;
    std::vector<MaterialPtr> materials;
    std::vector<GPUSkinnedMeshRenderer::SkinPtr> skins;
    
    GLTFLoader(MTL::Device* device);
    
    void loadFromFile(std::string filename, EntityPtr defaultSceneRoot, float scale = 1.0f);

private:
    void loadNode(EntityPtr parent, const tinygltf::Node& node, uint32_t nodeIndex,
                  const tinygltf::Model& model, float globalscale);

    void loadImages(tinygltf::Model& gltfModel);

    void loadMaterials(tinygltf::Model& gltfModel);

    void loadSkins(tinygltf::Model& gltfModel);
    
    void loadAnimations(tinygltf::Model& gltfModel);
    
private:
    MTL::Device* _device{nullptr};
    TextureLoader _resourceLoader;
    
    std::map<uint32_t, std::pair<EntityPtr, int32_t>> _linearNodes{};//index, (entity, skinIndex)
    bool _metallicRoughnessWorkflow = true;
    std::string _path;
    EntityPtr _defaultSceneRoot{nullptr};    
};


}
}

#endif /* gltf_loader_hpp */
