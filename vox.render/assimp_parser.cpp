//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/assimp_parser.h"

#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "vox.base/logging.h"
#include "vox.render/entity.h"
#include "vox.render/image_manager.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/material/pbr_specular_material.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/platform/filesystem.h"

namespace vox {
AssimpParser::AssimpParser(wgpu::Device &device) : _device(device) {}

void AssimpParser::loadModel(Entity *root, const std::string &file, unsigned int p_flags) {
    // retrieve the directory path of the filepath
    _directory = file.substr(0, file.find_last_of('/'));

    Assimp::Importer importer;
    const aiScene *scene =
            importer.ReadFile(fs::path::Get(fs::path::Type::ASSETS) + file, p_flags | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE("ERROR::ASSIMP:: {}", importer.GetErrorString())
    }

    // process ASSIMP's root node recursively
    processNode(root, scene->mRootNode, scene);
}

void AssimpParser::processNode(Entity *root, aiNode *node, const aiScene *scene) {
    auto entity = root->createChild();
    entity->name = node->mName.C_Str();
    auto &transform = node->mTransformation;
    entity->transform->setLocalMatrix(Matrix4x4F(transform.a1, transform.b1, transform.c1, transform.d1, transform.a2,
                                                 transform.b2, transform.c2, transform.d2, transform.a3, transform.b3,
                                                 transform.c3, transform.d3, transform.a4, transform.b4, transform.c4,
                                                 transform.d4));

    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        processMesh(entity, scene->mMeshes[node->mMeshes[i]], scene);
    }

    // after we've processed all the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(entity, node->mChildren[i], scene);
    }
}

void AssimpParser::processMesh(Entity *root, aiMesh *mesh, const aiScene *scene) {
    auto renderer = root->addComponent<MeshRenderer>();
    auto model_mesh = MeshManager::GetSingleton().LoadModelMesh();
    renderer->setMesh(model_mesh);

    std::vector<Vector3F> vec3_array(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vec3_array[i].x = mesh->mVertices[i].x;
        vec3_array[i].y = mesh->mVertices[i].y;
        vec3_array[i].z = mesh->mVertices[i].z;
    }
    model_mesh->setPositions(vec3_array);

    if (mesh->HasNormals()) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vec3_array[i].x = mesh->mNormals[i].x;
            vec3_array[i].y = mesh->mNormals[i].y;
            vec3_array[i].z = mesh->mNormals[i].z;
        }
        model_mesh->setNormals(vec3_array);
    }

    if (mesh->mTextureCoords[0]) {
        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
        // use models where a vertex can have multiple texture coordinates. so we always take the first set (0).
        std::vector<Vector2F> vec2_array(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vec2_array[i].x = mesh->mTextureCoords[0][i].x;
            vec2_array[i].y = mesh->mTextureCoords[0][i].y;
        }
        model_mesh->setUVs(vec2_array);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vec3_array[i].x = mesh->mTangents[i].x;
            vec3_array[i].y = mesh->mTangents[i].y;
            vec3_array[i].z = mesh->mTangents[i].z;
        }
        // todo

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vec3_array[i].x = mesh->mBitangents[i].x;
            vec3_array[i].y = mesh->mBitangents[i].y;
            vec3_array[i].z = mesh->mBitangents[i].z;
        }
        // todo
    }

    if (mesh->mColors[0]) {
        std::vector<Color> color_array(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            color_array[i].r = mesh->mColors[0][i].r;
            color_array[i].g = mesh->mColors[0][i].g;
            color_array[i].b = mesh->mColors[0][i].b;
            color_array[i].a = mesh->mColors[0][i].a;
        }
        model_mesh->setColors(color_array);
    }

    std::vector<uint32_t> indices;
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex
    // indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }
    model_mesh->setIndices(indices);
    model_mesh->addSubMesh(0, static_cast<uint32_t>(indices.size()));
    model_mesh->uploadData(true);

    const auto &min = mesh->mAABB.mMin;
    const auto &max = mesh->mAABB.mMax;
    model_mesh->bounds.upper_corner = Point3F(max.x, max.y, max.z);
    model_mesh->bounds.lower_corner = Point3F(min.x, min.y, min.z);

    // process materials
    renderer->setMaterial(processMaterial(scene->mMaterials[mesh->mMaterialIndex]));
}

std::shared_ptr<Material> AssimpParser::processMaterial(aiMaterial *material) {
    std::shared_ptr<Material> result{nullptr};
    aiShadingMode mode;
    material->Get(AI_MATKEY_SHADING_MODEL, mode);
    switch (mode) {
        case aiShadingMode_Unlit: {
            auto mat = std::make_shared<UnlitMaterial>(_device);
            aiColor4D color;
            auto info = material->Get(AI_MATKEY_BASE_COLOR, color);
            if (info == AI_SUCCESS) {
                mat->setBaseColor(Color(color.r, color.g, color.b, color.a));
            }

            mat->setBaseTexture(processTextures(material, aiTextureType_DIFFUSE));
            result = mat;
            break;
        }

        case aiShadingMode_PBR_BRDF: {
            float factor;
            auto info = material->Get(AI_MATKEY_ROUGHNESS_FACTOR, factor);
            if (info == AI_FAILURE) {
                auto mat = std::make_shared<PBRSpecularMaterial>(_device);
                float value;
                info = material->Get(AI_MATKEY_GLOSSINESS_FACTOR, value);
                if (info == AI_SUCCESS) {
                    mat->setGlossiness(value);
                }

                aiColor3D color;
                info = material->Get(AI_MATKEY_SPECULAR_FACTOR, color);
                if (info == AI_SUCCESS) {
                    mat->setSpecularColor(Color(color.r, color.g, color.b, 1.0));
                }
                mat->setBaseTexture(processTextures(material, aiTextureType_DIFFUSE));
                mat->setNormalTexture(processTextures(material, aiTextureType_NORMALS));
                mat->setEmissiveTexture(processTextures(material, aiTextureType_EMISSIVE));
                mat->setOcclusionTexture(processTextures(material, aiTextureType_AMBIENT_OCCLUSION));
                mat->setSpecularGlossinessTexture(processTextures(material, aiTextureType_DIFFUSE));
                result = mat;
            } else {
                auto mat = std::make_shared<PBRMaterial>(_device);
                float value;
                info = material->Get(AI_MATKEY_METALLIC_FACTOR, value);
                if (info == AI_SUCCESS) {
                    mat->setMetallic(value);
                }

                info = material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value);
                if (info == AI_SUCCESS) {
                    mat->setRoughness(value);
                }
                mat->setBaseTexture(processTextures(material, aiTextureType_DIFFUSE));
                mat->setNormalTexture(processTextures(material, aiTextureType_NORMALS));
                mat->setEmissiveTexture(processTextures(material, aiTextureType_EMISSIVE));
                mat->setOcclusionTexture(processTextures(material, aiTextureType_AMBIENT_OCCLUSION));
                mat->setMetallicRoughnessTexture(processTextures(material, aiTextureType_DIFFUSE_ROUGHNESS));
                result = mat;
            }
            break;
        }

        case aiShadingMode_Blinn:
        case aiShadingMode_Phong: {
            auto mat = std::make_shared<BlinnPhongMaterial>(_device);
            float value;
            auto info = material->Get(AI_MATKEY_SHININESS, value);
            if (info == AI_SUCCESS) {
                mat->setShininess(value);
            }

            aiColor4D color;
            info = material->Get(AI_MATKEY_BASE_COLOR, color);
            if (info == AI_SUCCESS) {
                mat->setBaseColor(Color(color.r, color.g, color.b, color.a));
            }
            mat->setBaseTexture(processTextures(material, aiTextureType_DIFFUSE));
            mat->setNormalTexture(processTextures(material, aiTextureType_NORMALS));
            mat->setEmissiveTexture(processTextures(material, aiTextureType_EMISSIVE));
            mat->setSpecularTexture(processTextures(material, aiTextureType_SPECULAR));
            result = mat;
            break;
        }

        default: {
            LOGI("Unknown material type: {}, use Blinn-Phong as default mat", toString(mode))
            auto mat = std::make_shared<BlinnPhongMaterial>(_device);
            float value;
            auto info = material->Get(AI_MATKEY_SHININESS, value);
            if (info == AI_SUCCESS) {
                mat->setShininess(value);
            }

            aiColor4D color;
            info = material->Get(AI_MATKEY_BASE_COLOR, color);
            if (info == AI_SUCCESS) {
                mat->setBaseColor(Color(color.r, color.g, color.b, color.a));
            }
            mat->setBaseTexture(processTextures(material, aiTextureType_DIFFUSE));
            mat->setNormalTexture(processTextures(material, aiTextureType_NORMALS));
            mat->setNormalTexture(processTextures(material, aiTextureType_HEIGHT));
            mat->setEmissiveTexture(processTextures(material, aiTextureType_EMISSIVE));
            mat->setSpecularTexture(processTextures(material, aiTextureType_SPECULAR));
            result = mat;

            break;
        }
    }

    return result;
}

std::shared_ptr<Image> AssimpParser::processTextures(aiMaterial *mat, aiTextureType type) {
    auto count = mat->GetTextureCount(type);
    if (count > 0) {
        aiString str;
        mat->GetTexture(type, 0, &str);
        std::string filename(str.C_Str());
        filename = _directory + '/' + filename;
        return ImageManager::GetSingleton().loadTexture(filename);
    }
    return nullptr;
}

std::string AssimpParser::toString(aiShadingMode mode) {
    switch (mode) {
        case aiShadingMode_Flat:
            return "Flat shading.";

        case aiShadingMode_Gouraud:
            return "Simple Gouraud shading.";

        case aiShadingMode_Phong:
            return "Phong shading.";

        case aiShadingMode_Blinn:
            return "Phong-Blinn shading.";

        case aiShadingMode_Toon:
            return "Toon shading.";

        case aiShadingMode_OrenNayar:
            return "OrenNayar shading.";

        case aiShadingMode_Minnaert:
            return "Minnaert shading.";

        case aiShadingMode_CookTorrance:
            return "CookTorrance shading.";

        case aiShadingMode_Unlit:
            return "Unlit shading.";

        case aiShadingMode_Fresnel:
            return "Fresnel shading.";

        case aiShadingMode_PBR_BRDF:
            return "Physically-Based Rendering (PBR) shading.";

        default:
            return "Shading Limit.";
    }
}

}  // namespace vox
