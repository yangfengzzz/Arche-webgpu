//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "atomic_compute_app.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "image/stb.h"
#include "shaderlib/wgsl_cache.h"
#include "shaderlib/wgsl_unlit.h"

namespace vox {
namespace {
class WGSLAtomicFragment : public WGSLCache {
public:
    WGSLAtomicFragment() : _uvShare("VertexOut") {}
    
private:
    WGSLUVShare _uvShare;
    
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override {
        _source.clear();
        _bindGroupInfo.clear();
        auto inputStructCounter = WGSLEncoder::startCounter(0);
        {
            auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
            _uvShare(encoder, macros, inputStructCounter);
            
            encoder.addStorageBufferBinding("u_atomic", UniformType::U32, true);
            encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
            
            encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
                source += "var counter:f32 = f32(u_atomic % 255u);\n";
                source += "out.finalColor = vec4<f32>(counter / 255.0, 1.0 - counter / 255.0, counter / 255.0, 1.0);\n";
            });
            encoder.flush();
        }
        WGSLEncoder::endCounter(inputStructCounter);
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
        shaderData.setBufferFunctor(_atomicProp, [this]()->Buffer {
            return _atomicBuffer;
        });
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
            encoder.addStruct("struct Counter {\n"
                              "counter: atomic<u32>;\n"
                              "}\n");
            encoder.addStorageBufferBinding("u_atomic", "Counter", false);
            
            encoder.addEntry({2, 2, 2}, [&](std::string &source){
                // source += "atomicStore(&u_atomic.counter, 0u);\n";
                // source += "storageBarrier();\n";
                source += "atomicAdd(&u_atomic.counter, 1u);\n";
            });
            encoder.flush();
        }
        _sourceCache[hash] = _source;
        _infoCache[hash] = _bindGroupInfo;
    }
};

} // namespace

//MARK: - AtomicComputeApp
void AtomicComputeApp::loadScene() {
    Shader::create("atomicRender", std::make_unique<WGSLUnlitVertex>(), std::make_unique<WGSLAtomicFragment>());
    
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
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

bool AtomicComputeApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    _pass = std::make_unique<ComputePass>(_device, std::make_unique<WGSLAtomicCompute>());
    _pass->setDispatchCount(1, 1, 1);
    _pass->attachShaderData(&_material->shaderData);
    
    return true;
}

void AtomicComputeApp::updateGPUTask(wgpu::CommandEncoder& commandEncoder) {
    ForwardApplication::updateGPUTask(commandEncoder);
    
    auto encoder = commandEncoder.BeginComputePass();
    _pass->compute(encoder);
    encoder.End();
}

}
