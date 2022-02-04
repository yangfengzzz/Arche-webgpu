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
#include "graphics/mesh.h"
#include "shader_common.h"
#include <iostream>

namespace vox {
namespace loader {
namespace {
bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex,
                            std::string* error, std::string* warning, int req_width, int req_height,
                            const unsigned char* bytes, int size, void* userData) {
    // This function will be used for samples that don't require images to be loaded
    return true;
}
}

GLTFLoader::GLTFLoader(MTL::Device* device):
_device(device),
_resourceLoader(TextureLoader(*device)) {
    
}

void GLTFLoader::loadFromFile(std::string filename, EntityPtr defaultSceneRoot, float scale) {
    _defaultSceneRoot = defaultSceneRoot;

    size_t pos = filename.find_last_of('/');
    _path = filename.substr(0, pos);
    
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
    
    std::string error, warning;
    
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);
    if (fileLoaded) {
        loadImages(gltfModel);
        loadMaterials(gltfModel);
        
        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            loadNode(nullptr, node, scene.nodes[i], gltfModel, scale);
        }
        
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
    else {
        // TODO: throw
        std::cerr << "Could not load glTF file \"" + filename + "\": " + error << std::endl;
        return;
    }
    
    for (auto extension : gltfModel.extensionsUsed) {
        if (extension == "KHR_materials_pbrSpecularGlossiness") {
            std::cout << "Required extension: " << extension;
            _metallicRoughnessWorkflow = false;
        }
    }
}

void GLTFLoader::loadNode(EntityPtr parent, const tinygltf::Node& node, uint32_t nodeIndex,
                          const tinygltf::Model& model, float globalscale) {
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
    
    // Node with children
    if (node.children.size() > 0) {
        for (auto i = 0; i < node.children.size(); i++) {
            loadNode(newNode, model.nodes[node.children[i]],
                     node.children[i], model, globalscale);
        }
    }
    
    // Node contains mesh data
    if (node.mesh > -1) {
        size_t vertexStart = 0;
        size_t currentNum = 0;
        std::vector<float> vertexBuffer{};
        
        auto bound = BoundingBox3F();
        MTL::VertexDescriptor vertexDescriptor;
        std::vector<Submesh> submeshes{};
        
        const tinygltf::Mesh mesh = model.meshes[node.mesh];
        auto renderer = newNode->addComponent<GPUSkinnedMeshRenderer>();
        for (size_t j = 0; j < mesh.primitives.size(); j++) {
            const tinygltf::Primitive &primitive = mesh.primitives[j];
            if (primitive.indices < 0) {
                continue;
            }
            
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
                
                vertexDescriptor.attributes[Attributes::Position].format(MTL::VertexFormatFloat3);
                vertexDescriptor.attributes[Attributes::Position].offset(0);
                vertexDescriptor.attributes[Attributes::Position].bufferIndex(0);

                size_t offset = 12;
                size_t elementCount = 3;
                if (bufferNormals) {
                    vertexDescriptor.attributes[Attributes::Normal].format(MTL::VertexFormatFloat3);
                    vertexDescriptor.attributes[Attributes::Normal].offset(offset);
                    vertexDescriptor.attributes[Attributes::Normal].bufferIndex(0);
                    offset += sizeof(float) * 3;
                    elementCount += 3;
                }
                
                if (bufferTexCoords) {
                    vertexDescriptor.attributes[Attributes::UV_0].format(MTL::VertexFormatFloat2);
                    vertexDescriptor.attributes[Attributes::UV_0].offset(offset);
                    vertexDescriptor.attributes[Attributes::UV_0].bufferIndex(0);
                    offset += sizeof(float) * 2;
                    elementCount += 2;
                }
                
                if (bufferColors) {
                    vertexDescriptor.attributes[Attributes::Color_0].format(MTL::VertexFormatFloat4);
                    vertexDescriptor.attributes[Attributes::Color_0].offset(offset);
                    vertexDescriptor.attributes[Attributes::Color_0].bufferIndex(0);
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferTangents) {
                    vertexDescriptor.attributes[Attributes::Tangent].format(MTL::VertexFormatFloat4);
                    vertexDescriptor.attributes[Attributes::Tangent].offset(offset);
                    vertexDescriptor.attributes[Attributes::Tangent].bufferIndex(0);
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferJoints) {
                    vertexDescriptor.attributes[Attributes::Joints_0].format(MTL::VertexFormatFloat4);
                    vertexDescriptor.attributes[Attributes::Joints_0].offset(offset);
                    vertexDescriptor.attributes[Attributes::Joints_0].bufferIndex(0);
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                
                if (bufferWeights) {
                    vertexDescriptor.attributes[Attributes::Weights_0].format(MTL::VertexFormatFloat4);
                    vertexDescriptor.attributes[Attributes::Weights_0].offset(offset);
                    vertexDescriptor.attributes[Attributes::Weights_0].bufferIndex(0);
                    offset += sizeof(float) * 4;
                    elementCount += 4;
                }
                vertexDescriptor.layouts[0].stride(offset);
                
                currentNum = posAccessor.count;
                vertexBuffer.reserve(vertexBuffer.size() + posAccessor.count * elementCount);
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
                                std::cerr << "numColorComponents has problem" <<std::endl;
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
                
                std::vector<uint32_t> buf;
                buf.reserve(accessor.count);
                switch (accessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        uint32_t *buf32 = new uint32_t[accessor.count];
                        memcpy(buf32, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            buf.push_back(buf32[index] + static_cast<uint32_t>(vertexStart));
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        uint16_t *buf16 = new uint16_t[accessor.count];
                        memcpy(buf16, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            buf.push_back(buf16[index] + static_cast<uint32_t>(vertexStart));
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        uint8_t *buf8 = new uint8_t[accessor.count];
                        memcpy(buf8, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            buf.push_back(buf8[index] + static_cast<uint32_t>(vertexStart));
                        }
                        break;
                    }
                    default:
                        std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                        return;
                }
                auto iBuffer = MeshBuffer(_device->makeBuffer(buf.data(), buf.size() * sizeof(uint32_t)), 0, buf.size() * sizeof(uint32_t));
                submeshes.push_back(Submesh(MTL::PrimitiveTypeTriangle, MTL::IndexTypeUInt32, buf.size(), iBuffer));
            }
            
            vertexStart += currentNum;
            auto mat = primitive.material > -1 ? materials[primitive.material] : materials.back();
            renderer->setMaterial(j, mat);
        }
        std::vector<MeshBuffer> vBuffer;
        vBuffer.emplace_back(MeshBuffer(_device->makeBuffer(vertexBuffer.data(), vertexBuffer.size() * sizeof(float)),
                                        0, vertexBuffer.size() * sizeof(float), 0));
        auto newMesh = std::make_shared<Mesh>(submeshes, vBuffer, vertexDescriptor);
        newMesh->bounds = bound;
        renderer->setMesh(newMesh);
    }
}

void GLTFLoader::loadImages(tinygltf::Model& gltfModel) {
    for (tinygltf::Image &image : gltfModel.images) {
        textures.push_back(_resourceLoader.loadTexture(_path, image.uri));
    }
}

void GLTFLoader::loadMaterials(tinygltf::Model& gltfModel) {
    for (tinygltf::Material &mat : gltfModel.materials) {
        auto material = std::make_shared<PBRMaterial>();
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
    materials.push_back(std::make_shared<PBRMaterial>());
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
                        std::cout << "unknown type" << std::endl;
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
                std::cout << "weights not yet supported, skipping channel" << std::endl;
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
