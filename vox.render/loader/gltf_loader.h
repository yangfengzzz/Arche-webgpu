//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gltf_loader_hpp
#define gltf_loader_hpp

#include "mesh/gpu_skinned_mesh_renderer.h"
#include "entity.h"
#include "texture/sampled_texture2d.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE

#include <tiny_gltf.h>

namespace vox {
namespace loader {

class GLTFLoader {
public:
    std::vector<std::unique_ptr<Image>> images;
    std::vector<SampledTexture2DPtr> textures;
    std::vector<MaterialPtr> materials;
    std::vector<std::vector<std::pair<MeshPtr, MaterialPtr>>> renderers;
    std::vector<GPUSkinnedMeshRenderer::SkinPtr> skins;
    
    GLTFLoader(wgpu::Device &device);
    
    void loadFromFile(std::string filename, Entity *defaultSceneRoot, float scale = 1.0f);
    
private:
    void loadScene(tinygltf::Model &gltfModel);
    
    void loadNode(Entity *parent, const tinygltf::Node &node, uint32_t nodeIndex,
                  const tinygltf::Model &model);
    
    void loadImages(tinygltf::Model &gltfModel);
    
    void loadSampler(const tinygltf::Sampler &gltf_sampler, SampledTexture2DPtr texture) const;
    
    void loadTextures(tinygltf::Model &gltfModel);
    
    void loadMaterials(tinygltf::Model &gltfModel);
    
    void loadMeshes(tinygltf::Model &gltfModel);
    
    void loadSkins(tinygltf::Model &gltfModel);
    
    void loadAnimations(tinygltf::Model &gltfModel);
    
private:
    wgpu::Device &_device;
    
    std::string _path{};
    
    /// The extensions that the GLTFLoader can load mapped to whether they should be enabled or not
    static std::unordered_map<std::string, bool> _supportedExtensions;
    
    /**
     * @brief Checks if the GLTFLoader supports an extension, and that it is present in the glTF file
     * @param requested_extension The extension to check
     * @returns True if the loader knows how to load the extension and it is present in the glTF file, false if not
     */
    bool isExtensionEnabled(const std::string &requested_extension);
    
    /**
     * @brief Finds whether an extension exists inside a tinygltf extension map and returns the result
     * @param tinygltf_extensions The extension map associated with a given tinygltf object
     * @param extension The extension to check
     * @returns A pointer to the value of the extension object, nullptr if it isn't found
     */
    tinygltf::Value *getExtension(tinygltf::ExtensionMap &tinygltf_extensions, const std::string &extension);
    
private:
    std::map<uint32_t, std::pair<Entity *, int32_t>> _linearNodes{};//index, (entity, skinIndex)
    bool _metallicRoughnessWorkflow = true;
    
    Entity *_defaultSceneRoot{nullptr};
};


}
}

#endif /* gltf_loader_hpp */
