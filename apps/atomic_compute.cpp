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
#include "shaderlib/wgsl_unlit.h"

namespace vox {
namespace {
class WGSLAtomicFragment : public WGSLCache {
public:
    WGSLAtomicFragment() {}
    
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override {
        _source.clear();
        _bindGroupInfo.clear();
        {
            auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
            encoder.addStorageBufferBinding("u_counter", "atomic<u32>", true);
            
            encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
                source += "var atomic = float(u_atomic) % 255.0;";
                source += "return vec4<f32>(atomic/255.0, 1 - atomic/255.0, atomic/255.0, 1.0);";
            });
            encoder.flush();
        }
        _sourceCache[hash] = _source;
        _infoCache[hash] = _bindGroupInfo;
    }
};

class AtomicMaterial: public BaseMaterial {
private:
    ShaderProperty _atomicProp;
    Buffer _atomicBuffer;
    
public:
    AtomicMaterial(wgpu::Device& device):
    BaseMaterial(device, Shader::find("atomicRender")),
    _atomicProp(Shader::createProperty("u_atomic", ShaderDataGroup::Material)),
    _atomicBuffer(device, sizeof(uint32_t), wgpu::BufferUsage::Storage) {
        setAtomicBuffer(_atomicBuffer);
    }
    
    const Buffer& atomicBuffer() {
        return _atomicBuffer;
    }
    
    void setAtomicBuffer(const Buffer& buffer) {
        shaderData.setBuffer(_atomicProp, buffer);
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

} // namespace

bool AtomicComputeApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto subpass = std::make_unique<ComputeSubpass>(std::make_unique<WGSLAtomicCompute>());
    subpass->setDispatchCount(2, 2, 2);
    subpass->attachShaderData(&_material->shaderData);
    _renderPass->addSubpass(std::move(subpass));
    
    return true;
}

void AtomicComputeApp::loadScene(uint32_t width, uint32_t height) {
    Shader::create("atomicRender", std::make_unique<WGSLUnlitFragment>(), std::make_unique<WGSLAtomicFragment>());
    
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
