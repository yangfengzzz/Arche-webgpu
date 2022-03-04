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

class AtomicComputeSubpass: public ComputeSubpass {
private:
    wgpu::ShaderModule _atomicCompute;
    wgpu::ComputePipelineDescriptor _state;
    wgpu::Buffer _atomicBuffer;
    wgpu::BindGroupDescriptor _group;
    
    AtomicMaterial* _material{nullptr};
    
public:
    AtomicComputeSubpass(RenderContext *context,
                         Scene *scene,
                         Camera *camera):
    ComputeSubpass(context, scene, camera) {
    }
    
    void setAtomicMaterial(AtomicMaterial* mat) {
        _material = mat;
    }
    
    void prepare() override {
        _atomicCompute = _pass->resourceCache().requestShader("atomicCounter");
        _state.compute.module = _atomicCompute;
        _state.compute.entryPoint = "main";
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
