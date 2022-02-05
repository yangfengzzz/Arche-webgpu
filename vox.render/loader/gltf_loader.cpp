//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE

#include "gltf_loader.h"
#include "scene_animator.h"
#include "material/pbr_material.h"
#include "mesh/buffer_mesh.h"
#include "filesystem.h"
#include <glog/logging.h>

#define KHR_LIGHTS_PUNCTUAL_EXTENSION "KHR_lights_punctual"

namespace vox {
namespace loader {
namespace {
inline wgpu::FilterMode find_min_filter(int min_filter) {
    switch (min_filter) {
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return wgpu::FilterMode::Nearest;
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
            return wgpu::FilterMode::Linear;
        default:
            return wgpu::FilterMode::Linear;
    }
};

inline wgpu::FilterMode find_mipmap_mode(int min_filter) {
    switch (min_filter) {
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return wgpu::FilterMode::Nearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
            return wgpu::FilterMode::Linear;
        default:
            return wgpu::FilterMode::Linear;
    }
};

inline wgpu::FilterMode find_mag_filter(int mag_filter) {
    switch (mag_filter) {
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
            return wgpu::FilterMode::Nearest;
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
            return wgpu::FilterMode::Linear;
        default:
            return wgpu::FilterMode::Linear;
    }
};

inline wgpu::AddressMode find_wrap_mode(int wrap) {
    switch (wrap) {
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return wgpu::AddressMode::Repeat;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return wgpu::AddressMode::ClampToEdge;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return wgpu::AddressMode::MirrorRepeat;
        default:
            return wgpu::AddressMode::Repeat;
    }
};

}        // namespace
std::unordered_map<std::string, bool> GLTFLoader::_supportedExtensions = {
    {KHR_LIGHTS_PUNCTUAL_EXTENSION, false}};

bool GLTFLoader::isExtensionEnabled(const std::string &requested_extension) {
    auto it = _supportedExtensions.find(requested_extension);
    if (it != _supportedExtensions.end()) {
        return it->second;
    } else {
        return false;
    }
}

tinygltf::Value *GLTFLoader::getExtension(tinygltf::ExtensionMap &tinygltf_extensions, const std::string &extension) {
    auto it = tinygltf_extensions.find(extension);
    if (it != tinygltf_extensions.end()) {
        return &it->second;
    } else {
        return nullptr;
    }
}

GLTFLoader::GLTFLoader(wgpu::Device& device):
_device(device) {
    
}

void GLTFLoader::loadFromFile(std::string filename, EntityPtr defaultSceneRoot, float scale) {
    tinygltf::TinyGLTF gltfContext;
    std::string gltf_file = vox::fs::path::get(vox::fs::path::Type::Assets) + filename;
    
    std::string error, warning;
    tinygltf::Model gltfModel;
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);
    
    if (!fileLoaded) {
        LOG(ERROR) << "Failed to load gltf file " << filename.c_str() << std::endl;
        return;
    }
    
    if (!error.empty()) {
        LOG(ERROR) << "Error loading gltf model: " << error.c_str() << std::endl;
        return;
    }
    
    if (!warning.empty()) {
        LOG(WARNING) << warning.c_str() << std::endl;
    }
    
    _defaultSceneRoot = defaultSceneRoot;
    size_t pos = filename.find_last_of('/');
    _path = filename.substr(0, pos);
    
    loadScene(gltfModel);
}

void GLTFLoader::loadScene(tinygltf::Model& gltfModel) {
    // Check extensions
    for (auto &used_extension: gltfModel.extensionsUsed) {
        if (used_extension == "KHR_materials_pbrSpecularGlossiness") {
            _metallicRoughnessWorkflow = false;
        }
        
        auto it = _supportedExtensions.find(used_extension);
        
        // Check if extension isn't supported by the GLTFLoader
        if (it == _supportedExtensions.end()) {
            // If extension is required then we shouldn't allow the scene to be loaded
            if (std::find(gltfModel.extensionsRequired.begin(), gltfModel.extensionsRequired.end(), used_extension) != gltfModel.extensionsRequired.end()) {
                throw std::runtime_error("Cannot load glTF file. Contains a required unsupported extension: " + used_extension);
            } else {
                // Otherwise, if extension isn't required (but is in the file) then print a warning to the user
                LOG(WARNING) << "glTF file contains an unsupported extension, unexpected results may occur: " << used_extension << std::endl;;
            }
        } else {
            // Extension is supported, so enable it
            LOG(INFO) << "glTF file contains extension: " << used_extension << std::endl;;
            it->second = true;
        }
    }
    
    // Load lights
    
    // Load images
    loadImages(gltfModel);
    
    // Load textures && samplers
    loadTextures(gltfModel);
    
    // Load materials
    loadMaterials(gltfModel);
    
    // Load meshes
    loadMeshes(gltfModel);
    
    // Load cameras
    
    // Load nodes && scenes
    const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
    for (size_t i = 0; i < scene.nodes.size(); i++) {
        const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
        loadNode(nullptr, node, scene.nodes[i], gltfModel);
    }
    
    // Load animations
    if (gltfModel.animations.size() > 0) {
        loadAnimations(gltfModel);
    }
    loadSkins(gltfModel);
    for (auto node : _linearNodes) {
        // Assign skins
        if (node.second.second > -1) {
            node.second.first->getComponent<GPUSkinnedMeshRenderer>()->setSkin(skins[node.second.second]);
        }
        // Initial pose
        auto mesh = node.second.first->getComponent<GPUSkinnedMeshRenderer>();
        if (mesh) {
            mesh->update(0);
        }
    }
}


void GLTFLoader::loadNode(EntityPtr parent, const tinygltf::Node& node, uint32_t nodeIndex,
                          const tinygltf::Model& model) {
    EntityPtr newNode = nullptr;
    if (parent) {
        newNode = parent->createChild(node.name);
    } else {
        newNode = _defaultSceneRoot->createChild(node.name);
    }
    _linearNodes[nodeIndex] = std::make_pair(newNode, node.skin);
    
    // Generate local node matrix
    if (node.translation.size() == 3) {
        auto translation = node.translation;
        newNode->transform->setPosition(Point3F(translation[0], translation[1], translation[2]));
    }
    if (node.rotation.size() == 4) {
        auto rotation = node.rotation;
        newNode->transform->setRotationQuaternion(QuaternionF(rotation[0], rotation[1], rotation[2], rotation[3]));
    }
    if (node.scale.size() == 3) {
        auto scale = node.scale;
        newNode->transform->setScale(Vector3F(scale[0], scale[1], scale[2]));
    }
    if (node.matrix.size() == 16) {
        auto m = Matrix4x4F();
        std::copy(node.matrix.begin(), node.matrix.end(), m.data());
        newNode->transform->setLocalMatrix(m);
    }
    
    if (node.mesh > 0) {
        const auto& meshMats = renderers[node.mesh];
        for (const auto& meshMat : meshMats) {
            auto renderer = newNode->addComponent<GPUSkinnedMeshRenderer>();
            renderer->setMesh(meshMat.first);
            renderer->setMaterial(meshMat.second);
        }
    }
    
    // Node with children
    if (node.children.size() > 0) {
        for (auto i = 0; i < node.children.size(); i++) {
            loadNode(newNode, model.nodes[node.children[i]],
                     node.children[i], model);
        }
    }
}

void GLTFLoader::loadImages(tinygltf::Model& gltfModel) {
    for (tinygltf::Image &gltf_image : gltfModel.images) {
        std::unique_ptr<Image> image{nullptr};
        
        if (!gltf_image.image.empty()) {
            // Image embedded in gltf file
            auto mipmap = Mipmap{
                /* .level = */ 0,
                /* .offset = */ 0,
                /* .extent = */ {/* .width = */ static_cast<uint32_t>(gltf_image.width),
                    /* .height = */ static_cast<uint32_t>(gltf_image.height),
                    /* .depth = */ 1u}};
            std::vector<Mipmap> mipmaps{mipmap};
            image = std::make_unique<Image>(std::move(gltf_image.image), std::move(mipmaps));
        } else {
            // Load image from uri
            auto image_uri = _path + "/" + gltf_image.uri;
            image = Image::load(image_uri);
        }
        images.emplace_back(std::move(image));
    }
}

void GLTFLoader::loadSampler(const tinygltf::Sampler &gltf_sampler, SampledTexture2DPtr texture) const {
    texture->setAddressModeU(find_wrap_mode(gltf_sampler.wrapS));
    texture->setAddressModeV(find_wrap_mode(gltf_sampler.wrapT));
    
    texture->setMipmapFilter(find_mipmap_mode(gltf_sampler.minFilter));
    texture->setMinFilterMode(find_min_filter(gltf_sampler.minFilter));
    texture->setMagFilterMode(find_mag_filter(gltf_sampler.magFilter));
}

void GLTFLoader::loadTextures(tinygltf::Model& gltfModel) {
    for (auto &gltf_texture: gltfModel.textures) {
        auto image = images.at(gltf_texture.source).get();
        SampledTexture2DPtr texture = image->createSampledTexture(_device);
        if (gltf_texture.sampler >= 0) {
            loadSampler(gltfModel.samplers.at(gltf_texture.sampler), texture);
        }
        textures.emplace_back(std::move(texture));
    }
}

void GLTFLoader::loadMaterials(tinygltf::Model& gltfModel) {
    for (tinygltf::Material &mat : gltfModel.materials) {
        auto material = std::make_shared<PBRMaterial>(_device);
        if (mat.values.find("baseColorTexture") != mat.values.end()) {
            material->setBaseTexture(textures[gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source]);
        }
        // Metallic roughness workflow
        if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
            material->setMetallicRoughnessTexture(textures[gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source]);
        }
        if (mat.values.find("roughnessFactor") != mat.values.end()) {
            material->setRoughness(static_cast<float>(mat.values["roughnessFactor"].Factor()));
        }
        if (mat.values.find("metallicFactor") != mat.values.end()) {
            material->setMetallic(static_cast<float>(mat.values["metallicFactor"].Factor()));
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            auto color = mat.values["baseColorFactor"].ColorFactor();
            material->setBaseColor(Color(color[0], color[1], color[2], color[3]));
        }
        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
            material->setNormalTexture(textures[gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source]);
        }
        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
            material->setEmissiveTexture(textures[gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source]);
        }
        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
            material->setOcclusionTexture(textures[gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source]);
        }
        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
            if (param.string_value == "BLEND") {
                material->setBlendMode(BlendMode::Normal);
            }
            if (param.string_value == "MASK") {
                material->setBlendMode(BlendMode::Additive);
            }
        }
        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
            material->setAlphaCutoff(static_cast<float>(mat.additionalValues["alphaCutoff"].Factor()));
        }
        
        materials.push_back(material);
    }
    // Push a default material at the end of the list for meshes with no material assigned
    materials.push_back(std::make_shared<PBRMaterial>(_device));
}

void GLTFLoader::loadMeshes(tinygltf::Model& model) {
    for (auto &gltf_mesh: model.meshes) {
        std::vector<std::pair<MeshPtr, MaterialPtr>> renderer{};
        for (auto &primitive: gltf_mesh.primitives) {
            if (primitive.indices < 0) {
                continue;
            }
            
            auto bound = BoundingBox3F();
            std::vector<float> vertexBuffer{};
            std::vector<uint32_t> indexBuffer{};
            std::vector<wgpu::VertexAttribute> vertexAttributes{};
            wgpu::VertexBufferLayout vertexBufferLayout;
            // Vertices
            {
                const float *bufferPos = nullptr;
                const float *bufferNormals = nullptr;
                const float *bufferTexCoords = nullptr;
                const float* bufferColors = nullptr;
                const float *bufferTangents = nullptr;
                uint32_t numColorComponents = 0;
                const uint16_t *bufferJoints = nullptr;
                const float *bufferWeights = nullptr;
                
                // Position attribute is required
                assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
                
                const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                Point3F posMin = Point3F(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                Point3F posMax = Point3F(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
                bound.merge(BoundingBox3F(posMin, posMax));
                
                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                    bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                }
                
                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferTexCoords = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }
                
                if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                    const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
                    const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
                    // Color buffer are either of type vec3 or vec4
                    numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
                    bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                }
                
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
                    const tinygltf::Accessor &tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView &tangentView = model.bufferViews[tangentAccessor.bufferView];
                    bufferTangents = reinterpret_cast<const float *>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                }
                
                // Skinning
                // Joints
                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
                    // TODO data type have problem
                    const tinygltf::Accessor &jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
                    const tinygltf::BufferView &jointView = model.bufferViews[jointAccessor.bufferView];
                    bufferJoints = reinterpret_cast<const uint16_t *>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
                }
                
                if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferWeights = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }
                
                wgpu::VertexAttribute attr;
                attr.offset = 0;
                attr.format = wgpu::VertexFormat::Float32x3;
                attr.shaderLocation = (uint32_t)Attributes::Position;
                vertexAttributes.push_back(attr);
                
                size_t offset = 12;
                size_t elementCount = 3;
                if (bufferNormals) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x3;
                    attr.shaderLocation = (uint32_t)Attributes::Normal;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 3;
                    elementCount += 3;
                }
                
                if (bufferTexCoords) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x2;
                    attr.shaderLocation = (uint32_t)Attributes::UV_0;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 2;
                    elementCount += 2;
                }
                
                if (bufferColors) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x4;
                    attr.shaderLocation = (uint32_t)Attributes::Color_0;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferTangents) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x4;
                    attr.shaderLocation = (uint32_t)Attributes::Tangent;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferJoints) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x4;
                    attr.shaderLocation = (uint32_t)Attributes::Joints_0;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferWeights) {
                    attr.offset = offset;
                    attr.format = wgpu::VertexFormat::Float32x4;
                    attr.shaderLocation = (uint32_t)Attributes::Weights_0;
                    vertexAttributes.push_back(attr);
                    
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                vertexBufferLayout.attributes = vertexAttributes.data();
                vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttributes.size());
                vertexBufferLayout.arrayStride = offset;
                
                vertexBuffer.reserve(posAccessor.count * elementCount);
                for (size_t v = 0; v < posAccessor.count; v++) {
                    vertexBuffer.insert(vertexBuffer.end(), &bufferPos[v * 3], &bufferPos[v * 3 + 3]);
                    if (bufferNormals) {
                        vertexBuffer.insert(vertexBuffer.end(), &bufferNormals[v * 3], &bufferNormals[v * 3 + 3]);
                    }
                    if (bufferTexCoords) {
                        vertexBuffer.insert(vertexBuffer.end(), &bufferTexCoords[v * 2], &bufferTexCoords[v * 2 + 2]);
                    }
                    if (bufferColors) {
                        switch (numColorComponents) {
                            case 3:
                                vertexBuffer.insert(vertexBuffer.end(), &bufferColors[v * 3], &bufferColors[v * 3 + 3]);
                                vertexBuffer.push_back(1.0);
                            case 4:
                                vertexBuffer.insert(vertexBuffer.end(), &bufferColors[v * 4], &bufferColors[v * 4 + 4]);
                            default:
                                LOG(ERROR) << "numColorComponents has problem" <<std::endl;
                        }
                    }
                    if (bufferTangents) {
                        vertexBuffer.insert(vertexBuffer.end(), &bufferTangents[v * 4], &bufferTangents[v * 4 + 4]);
                    }
                    if (bufferJoints) {
                        vertexBuffer.insert(vertexBuffer.end(), &bufferJoints[v * 4], &bufferJoints[v * 4 + 4]);
                    }
                    if (bufferWeights) {
                        vertexBuffer.insert(vertexBuffer.end(), &bufferWeights[v * 4], &bufferWeights[v * 4 + 4]);
                    }
                }
            }
            // Indices
            {
                const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
                
                indexBuffer.reserve(accessor.count);
                switch (accessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        std::vector<uint32_t> buf32(accessor.count);
                        memcpy(buf32.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf32[index]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        std::vector<uint16_t> buf16(accessor.count);
                        memcpy(buf16.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf16[index]);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        std::vector<uint8_t> buf8(accessor.count);
                        memcpy(buf8.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf8[index]);
                        }
                        break;
                    }
                    default:
                        LOG(ERROR) << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                        return;
                }
            }
            auto bufferMesh = std::make_shared<BufferMesh>();
            auto iBuffer = Buffer(_device, indexBuffer.data(), indexBuffer.size() * sizeof(uint32_t), wgpu::BufferUsage::Index);
            auto vBuffer = Buffer(_device, vertexBuffer.data(), vertexBuffer.size() * sizeof(float), wgpu::BufferUsage::Vertex);
            bufferMesh->setVertexBufferBinding(vBuffer);
            bufferMesh->setIndexBufferBinding(iBuffer, wgpu::IndexFormat::Uint32);
            bufferMesh->setVertexLayouts({vertexBufferLayout});
            bufferMesh->addSubMesh(0, static_cast<uint32_t>(indexBuffer.size()), wgpu::PrimitiveTopology::TriangleList);
            bufferMesh->bounds = bound;
            renderer.emplace_back(std::make_pair(bufferMesh, primitive.material > -1 ? materials[primitive.material] : materials.back()));
        }
        renderers.push_back(renderer);
    }
}

void GLTFLoader::loadSkins(tinygltf::Model& gltfModel) {
    for (tinygltf::Skin &source : gltfModel.skins) {
        GPUSkinnedMeshRenderer::SkinPtr newSkin = std::make_shared<GPUSkinnedMeshRenderer::Skin>();
        newSkin->name = source.name;
        
        // Find joint nodes
        for (int jointIndex : source.joints) {
            EntityPtr node = _linearNodes[jointIndex].first;
            if (node) {
                newSkin->joints.push_back(node.get());
            }
        }
        
        // Get inverse bind matrices from buffer
        if (source.inverseBindMatrices > -1) {
            const tinygltf::Accessor &accessor = gltfModel.accessors[source.inverseBindMatrices];
            const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
            newSkin->inverseBindMatrices.resize(accessor.count);
            memcpy(newSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(Matrix4x4F));
        }
        
        skins.push_back(std::move(newSkin));
    }
}

void GLTFLoader::loadAnimations(tinygltf::Model& gltfModel) {
    auto animator = _defaultSceneRoot->addComponent<SceneAnimator>();
    for (size_t i = 0; i < gltfModel.animations.size(); i++) {
        const auto& anim = gltfModel.animations[i];
        auto animation = std::make_unique<SceneAnimationClip>(anim.name.empty()? std::to_string(i):anim.name);
        
        // Samplers
        for (auto &samp : anim.samplers) {
            SceneAnimationClip::AnimationSampler sampler{};
            
            if (samp.interpolation == "LINEAR") {
                sampler.interpolation = SceneAnimationClip::AnimationSampler::InterpolationType::LINEAR;
            }
            if (samp.interpolation == "STEP") {
                sampler.interpolation = SceneAnimationClip::AnimationSampler::InterpolationType::STEP;
            }
            if (samp.interpolation == "CUBICSPLINE") {
                sampler.interpolation = SceneAnimationClip::AnimationSampler::InterpolationType::CUBICSPLINE;
            }
            
            // Read sampler input time values
            {
                const tinygltf::Accessor &accessor = gltfModel.accessors[samp.input];
                const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
                
                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                
                float *buf = new float[accessor.count];
                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float));
                for (size_t index = 0; index < accessor.count; index++) {
                    sampler.inputs.push_back(buf[index]);
                }
                
                for (auto input : sampler.inputs) {
                    if (input < animation->start()) {
                        animation->setStart(input);
                    };
                    if (input > animation->end()) {
                        animation->setEnd(input);
                    }
                }
            }
            
            // Read sampler output T/R/S values
            {
                const tinygltf::Accessor &accessor = gltfModel.accessors[samp.output];
                const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
                
                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                
                switch (accessor.type) {
                    case TINYGLTF_TYPE_VEC3: {
                        Vector3F *buf = new Vector3F[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(Vector3F));
                        for (size_t index = 0; index < accessor.count; index++) {
                            sampler.outputsVec4.push_back(Vector4F(buf[index], 0.0f));
                        }
                        break;
                    }
                    case TINYGLTF_TYPE_VEC4: {
                        Vector4F *buf = new Vector4F[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(Vector4F));
                        for (size_t index = 0; index < accessor.count; index++) {
                            sampler.outputsVec4.push_back(buf[index]);
                        }
                        break;
                    }
                    default: {
                        LOG(ERROR) << "unknown type" << std::endl;
                        break;
                    }
                }
            }
            
            animation->addSampler(sampler);
        }
        
        // Channels
        for (auto &source: anim.channels) {
            SceneAnimationClip::AnimationChannel channel{};
            
            if (source.target_path == "rotation") {
                channel.path = SceneAnimationClip::AnimationChannel::PathType::ROTATION;
            }
            if (source.target_path == "translation") {
                channel.path = SceneAnimationClip::AnimationChannel::PathType::TRANSLATION;
            }
            if (source.target_path == "scale") {
                channel.path = SceneAnimationClip::AnimationChannel::PathType::SCALE;
            }
            if (source.target_path == "weights") {
                LOG(WARNING) << "weights not yet supported, skipping channel" << std::endl;
                continue;
            }
            channel.samplerIndex = source.sampler;
            channel.node = _linearNodes[source.target_node].first;
            if (!channel.node) {
                continue;
            }
            
            animation->addChannel(channel);
        }
        animator->addAnimationClip(std::move(animation));
    }
}

}
}
