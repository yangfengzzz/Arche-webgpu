//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/atomic_compute_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
namespace {
class AtomicMaterial : public BaseMaterial {
private:
    const std::string _atomicProp;
    Buffer _atomicBuffer;

public:
    explicit AtomicMaterial(wgpu::Device& device)
        : BaseMaterial(device),
          _atomicProp("u_atomic"),
          _atomicBuffer(device, sizeof(uint32_t), wgpu::BufferUsage::Storage) {
        setAtomicBuffer(_atomicBuffer);
        vertex_source_ = ShaderManager::GetSingleton().LoadShader("base/unlit.vert");
        fragment_source_ = ShaderManager::GetSingleton().LoadShader("base/compute/atomic_counter.frag");
    }

    const Buffer& atomicBuffer() { return _atomicBuffer; }

    void setAtomicBuffer(const Buffer& buffer) {
        shaderData.setBufferFunctor(_atomicProp, [this]() -> Buffer { return _atomicBuffer; });
    }
};

}  // namespace

// MARK: - AtomicComputeApp
void AtomicComputeApp::loadScene() {
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

    scene->play();
}

bool AtomicComputeApp::prepare(Platform& platform) {
    ForwardApplication::prepare(platform);

    _pass = std::make_unique<ComputePass>(_device,
                                          ShaderManager::GetSingleton().LoadShader("base/compute/atomic_counter.comp"));
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

}  // namespace vox
