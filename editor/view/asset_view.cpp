//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_view.h"
#include "camera.h"
#include "rendering/subpasses/forward_subpass.h"
#include "grid.h"

#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"

namespace vox {
namespace editor {
namespace ui {
AssetView::AssetView(const std::string & p_title, bool p_opened,
                   const PanelWindowSettings & p_windowSettings,
                   RenderContext* renderContext, Scene* scene) :
View(p_title, p_opened, p_windowSettings, renderContext),
_scene(scene) {
    scene->background.solidColor = Color(0.2, 0.4, 0.6, 1.0);
    auto editorRoot = _scene->findEntityByName("AssetRoot");
    if (!editorRoot) {
        editorRoot = _scene->createRootEntity("AssetRoot");
    }
    loadScene(editorRoot);
    
    // scene render target
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        _renderPassDescriptor.colorAttachmentCount = 1;
        _renderPassDescriptor.colorAttachments = &_renderPassColorAttachments;
        _renderPassDescriptor.depthStencilAttachment = &_renderPassDepthStencilAttachment;
        
        _renderPassColorAttachments.storeOp = wgpu::StoreOp::Store;
        _renderPassColorAttachments.loadOp = wgpu::LoadOp::Clear;
        auto& color = scene->background.solidColor;
        _renderPassColorAttachments.clearValue = wgpu::Color{color.r, color.g, color.b, color.a};
        _renderPassDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
        _renderPassDepthStencilAttachment.depthClearValue = 1.0;
        _renderPassDepthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
        _renderPassDepthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
        _renderPassDepthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
        _renderPass = std::make_unique<RenderPass>(renderContext->device(), _renderPassDescriptor);
        _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext, _depthStencilTextureFormat,
                                                                 scene, _mainCamera));
    }
}

void AssetView::loadScene(EntityPtr& rootEntity) {
    auto cameraEntity = rootEntity->createChild("MainCamera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    rootEntity->addComponent<editor::Grid>();
    _mainCamera = cameraEntity->addComponent<Camera>();
    _cameraControl = cameraEntity->addComponent<control::OrbitControl>();
    
    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_renderContext->device());
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_renderContext->device(), cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
}

void AssetView::update(float deltaTime) {
    View::update(deltaTime);
    
    auto [winWidth, winHeight] = safeSize();
    if (winWidth > 0) {
        _mainCamera->setAspectRatio(float(winWidth) / float(winHeight));
        _mainCamera->resize(winWidth, winHeight, winWidth * 2, winHeight * 2);
    }
}

void AssetView::render(wgpu::CommandEncoder& commandEncoder) {
    if (isFocused()) {
        _cameraControl->setEnabled(true);
    } else {
        _cameraControl->setEnabled(false);
    }
    
    if (_texture && isFocused()) {
        _renderPassColorAttachments.view = _texture.CreateView();
        _renderPassDepthStencilAttachment.view = _depthStencilTexture;
        _renderPass->draw(commandEncoder);
    }
}



}
}
}
