//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "atomic_compute.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "image/stb.h"
#include "shaderlib/wgsl_cache.h"
#include "rendering/compute_subpass.h"

namespace vox {
namespace {
class AtomicMaterial: public BaseMaterial {
private:
    ShaderProperty _atomicProp;
    
public:
    AtomicMaterial(wgpu::Device& device):
    BaseMaterial(device, Shader::find("atomicRender")),
    _atomicProp(Shader::createProperty("u_atomic", ShaderDataGroup::Material)) {}
    
    void setAtomicBuffer(const wgpu::Buffer& buffer) {
        shaderData.setData(_atomicProp, buffer);
    }
};

class WGSLAtomicCompute : public WGSLCache {
public:
    WGSLAtomicCompute() {}
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override {
        _source.clear();
        _bindGroupInfo.clear();
        {
            auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
            encoder.addStorageBufferBinding("u_counter", "atomic<u32>", false);

            encoder.addEntry(Vector3F(2, 2, 2), {{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
                source += "atomicStore(&u_counter, 0u);";
                source += "storageBarrier()";
                source += "atomicAdd(&u_counter, 1u);";
            });
            encoder.flush();
        }
        _sourceCache[hash] = _source;
        _infoCache[hash] = _bindGroupInfo;
    }
};

class AtomicComputeSubpass: public ComputeSubpass {
private:
    wgpu::ShaderModule _atomicCompute;
    wgpu::ComputePipelineDescriptor _state;
    wgpu::Buffer _atomicBuffer;
    
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    
    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
    
    AtomicMaterial* _material{nullptr};
    
public:
    AtomicComputeSubpass(RenderContext *context,
                         Scene *scene,
                         Camera *camera):
    ComputeSubpass(std::make_unique<WGSLAtomicCompute>(), context, scene, camera) {
    }
    
    void setAtomicMaterial(AtomicMaterial* mat) {
        _material = mat;
    }
    
    void prepare() override {
        _atomicCompute = compileShader(ShaderMacroCollection());
        _state.compute.module = _atomicCompute;
        _state.compute.entryPoint = "main";
        
        std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
        for (auto& layoutDesc : _bindGroupLayoutDescriptorMap) {
            wgpu::BindGroupLayout bindGroupLayout = _pass->resourceCache().requestBindGroupLayout(layoutDesc.second);
            _bindGroupEntries.clear();
            _bindGroupEntries.resize(layoutDesc.second.entryCount);
            for (uint32_t i = 0; i < layoutDesc.second.entryCount; i++) {
                auto& entry = layoutDesc.second.entries[i];
                _bindGroupEntries[i].binding = entry.binding;
                if (entry.buffer.type != wgpu::BufferBindingType::Undefined) {
                    _bindingData(_bindGroupEntries[i], material, renderer);
                } else if (entry.texture.sampleType != wgpu::TextureSampleType::Undefined ||
                           entry.storageTexture.access != wgpu::StorageTextureAccess::Undefined) {
                    _bindingTexture(_bindGroupEntries[i], material, renderer);
                } else if (entry.sampler.type != wgpu::SamplerBindingType::Undefined) {
                    _bindingSampler(_bindGroupEntries[i], material, renderer);
                }
            }
            _bindGroupDescriptor.layout = bindGroupLayout;
            _bindGroupDescriptor.entryCount = layoutDesc.second.entryCount;
            _bindGroupDescriptor.entries = _bindGroupEntries.data();
            auto uniformBindGroup = _pass->resourceCache().requestBindGroup(_bindGroupDescriptor);
//            passEncoder.SetBindGroup(layoutDesc.first, uniformBindGroup);
            bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
        }
        flush();
        
        
        _material->setAtomicBuffer(_atomicBuffer);
    }
    
    void compute(wgpu::ComputePassEncoder &commandEncoder) override {
        auto pipeline = _pass->resourceCache().requestPipeline(_state);
        commandEncoder.SetPipeline(pipeline);
        auto group = _pass->resourceCache().requestBindGroup(_group);
        commandEncoder.SetBindGroup(0, group);
        commandEncoder.Dispatch(2);
    }
};

} // namespace

bool AtomicComputeApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto subpass = std::make_unique<AtomicComputeSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    subpass->setAtomicMaterial(static_cast<AtomicMaterial*>(_material.get()));
    _renderPass->addSubpass(std::move(subpass));
    
    return true;
}

void AtomicComputeApp::loadScene(uint32_t width, uint32_t height) {
    // Shader::create("atomicRender", "vertex_unlit", "fragment_atomic");
    
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    auto cubeEntity = rootEntity->createChild();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device, 1));
    _material = std::make_shared<AtomicMaterial>(_device);
    renderer->setMaterial(_material);
}
}
