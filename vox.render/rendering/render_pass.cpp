//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/render_pass.h"

#include "vox.base/logging.h"
#include "vox.render/rendering/subpass.h"

namespace vox {
RenderPass::RenderPass(wgpu::Device& device, wgpu::RenderPassDescriptor& desc) : _desc(desc), _resourceCache(device) {}

const wgpu::RenderPassDescriptor& RenderPass::renderPassDescriptor() { return _desc; }

// MARK: - Subpass
void RenderPass::draw(wgpu::CommandEncoder& commandEncoder, std::optional<std::string> label) {
    assert(!_subpasses.empty() && "Render pipeline should contain at least one sub-pass");

    wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_desc);
    if (label) {
        encoder.SetLabel(label.value().c_str());
    }
    for (size_t i = 0; i < _subpasses.size(); ++i) {
        _activeSubpassIndex = i;
        _subpasses[i]->draw(encoder);
    }
    _activeSubpassIndex = 0;
    encoder.End();
}

void RenderPass::addSubpass(std::unique_ptr<Subpass>&& subpass) {
    subpass->setRenderPass(this);
    _subpasses.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>>& RenderPass::subpasses() { return _subpasses; }

std::unique_ptr<Subpass>& RenderPass::activeSubpass() { return _subpasses[_activeSubpassIndex]; }

// MARK: - ParentPass
void RenderPass::addParentPass(const std::string& name, RenderPass* pass) {
    auto iter = _parentPass.find(name);
    if (iter == _parentPass.end()) {
        _parentPass[name] = pass;
    } else {
        LOGW("already exists")
    }
}

RenderPass* RenderPass::removeParentPass(const std::string& name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        auto pass = iter->second;
        _parentPass.erase(iter);
        return pass;
    } else {
        LOGW("can't find pass")
        return nullptr;
    }
}

RenderPass* RenderPass::findPass(const std::string& name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        return iter->second;
    } else {
        return nullptr;
    }
}

void RenderPass::clearParentPass() { _parentPass.clear(); }

ResourceCache& RenderPass::resourceCache() { return _resourceCache; }

}  // namespace vox
