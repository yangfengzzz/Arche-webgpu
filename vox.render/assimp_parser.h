//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <assimp/scene.h>

#include <string>

#include "vox.render/core/device.h"
#include "vox.render/scene_forward.h"
#include "vox.render/texture.h"

namespace vox {
class AssimpParser {
public:
    explicit AssimpParser(Device &device);

    /**
     * Simply load meshes from a file using assimp
     * Return true on success
     */
    void LoadModel(Entity *root, const std::string &file, unsigned int p_flags);

    /**
     * Processes a node in a recursive fashion.
     * Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
     */
    void ProcessNode(Entity *root, aiNode *node, const aiScene *scene);

    void ProcessMesh(Entity *root, aiMesh *mesh, const aiScene *scene);

    std::shared_ptr<Material> ProcessMaterial(aiMaterial *material);

    std::shared_ptr<Texture> ProcessTextures(aiMaterial *mat, aiTextureType type);

private:
    static std::string ToString(aiShadingMode mode);

    std::string directory_;
    Device &device_;
};

}  // namespace vox
