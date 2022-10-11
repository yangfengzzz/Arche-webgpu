//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/view/asset_view.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.toolkit/grid/grid_control.h"

namespace vox::editor::ui {
AssetView::AssetView(const std::string& p_title,
                     bool p_opened,
                     const PanelWindowSettings& p_windowSettings,
                     RenderContext* renderContext,
                     Scene* scene)
    : View(p_title, p_opened, p_windowSettings, renderContext), _scene(scene) {
    scene->background.solidColor = Color(0.2, 0.4, 0.6, 1.0);
    auto editorRoot = _scene->findEntityByName("AssetRoot");
    if (!editorRoot) {
        editorRoot = _scene->createRootEntity("AssetRoot");
    }
    loadScene(editorRoot);

    // scene render target
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored, so it can be displayed
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
        auto subpass =
                std::make_unique<GeometrySubpass>(_renderContext, _depthStencilTextureFormat, scene, _mainCamera);
        _subpass = subpass.get();
        //        _subpass->setRenderMode(ForwardSubpass::RenderMode::MANUAL);
        _renderPass->addSubpass(std::move(subpass));
    }

    //    _subpass->addRenderElement(_elements[0]);
    //    _subpass->addRenderElement(_elements[1]);
}

void AssetView::loadScene(Entity* rootEntity) {
    auto cameraEntity = rootEntity->createChild("MainCamera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _cameraControl = cameraEntity->addComponent<control::OrbitControl>();

    auto gridControl = rootEntity->addComponent<grid::GridControl>();
    gridControl->camera = _mainCamera;
//    _elements.emplace_back(grid, grid->mesh(), grid->mesh()->subMesh(), grid->getMaterial());

    // create box test entity
    float radius = 2.0;
    auto sphereEntity = rootEntity->createChild("SphereEntity");
    auto sphereMtl = std::make_shared<BlinnPhongMaterial>(_renderContext->device());
    sphereMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    auto _renderer = sphereEntity->addComponent<MeshRenderer>();
    _renderer->setMesh(PrimitiveMesh::createSphere(_renderContext->device(), radius));
    _renderer->setMaterial(sphereMtl);
    _renderer->shaderData.addDefine(HAS_UV);
    _renderer->shaderData.addDefine(HAS_NORMAL);
    _renderer->setEnabled(false);
    _elements.emplace_back(_renderer, _renderer->mesh(), _renderer->mesh()->subMesh(), _renderer->getMaterial());
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
    if (IsFocused()) {
        _cameraControl->onEnable();
    } else {
        _cameraControl->onDisable();
    }

    if (_texture) {
        _renderPassColorAttachments.view = _texture.CreateView();
        _renderPassDepthStencilAttachment.view = _depthStencilTexture;
        _renderPass->draw(commandEncoder);
    }
}

}  // namespace vox::editor::ui
