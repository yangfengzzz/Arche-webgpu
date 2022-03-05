//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_pass.h"
#include "render_subpass.h"
#include "compute_subpass.h"
#include <glog/logging.h>

namespace vox {
RenderPass::RenderPass(wgpu::Device& device, wgpu::RenderPassDescriptor& desc):
_desc(desc),
_resourceCache(device) {
}

const wgpu::RenderPassDescriptor& RenderPass::renderPassDescriptor() {
    return _desc;
}

void RenderPass::setGUI(GUI* gui) {
    _gui = gui;
}

//MARK: - Subpass
void RenderPass::draw(wgpu::CommandEncoder& commandEncoder,
                      std::optional<std::string> label) {
    assert(!_subpasses.empty() && "Render pipeline should contain at least one sub-pass");
    
    std::pair<Subpass::Type, std::any> preEncoder;
    auto encoderCreator = [&](Subpass* subpass) {
        if (subpass->type() == Subpass::Type::Render) {
            wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_desc);
            if (label) {
                encoder.SetLabel(label.value().c_str());
            }
            static_cast<RenderSubpass*>(subpass)->draw(encoder);
            
            preEncoder.first = Subpass::Type::Render;
            preEncoder.second = encoder;
        } else {
            wgpu::ComputePassEncoder encoder = commandEncoder.BeginComputePass();
            if (label) {
                encoder.SetLabel(label.value().c_str());
            }
            static_cast<ComputeSubpass*>(subpass)->compute(encoder);
            
            preEncoder.first = Subpass::Type::Compute;
            preEncoder.second = encoder;
        }
    };
    
    for (size_t i = 0; i < _subpasses.size(); ++i) {
        _activeSubpassIndex = i;
        auto subpass = _subpasses[i].get();
        if (i == 0) {
            encoderCreator(subpass);
            continue;
        }
        
        if (subpass->type() == preEncoder.first) {
            if (subpass->type() == Subpass::Type::Render) {
                auto encoder = std::any_cast<wgpu::RenderPassEncoder>(preEncoder.second);
                static_cast<RenderSubpass*>(subpass)->draw(encoder);
            } else {
                auto encoder = std::any_cast<wgpu::ComputePassEncoder>(preEncoder.second);
                static_cast<ComputeSubpass*>(subpass)->compute(encoder);
            }
        } else {
            if (preEncoder.first == Subpass::Type::Render) {
                std::any_cast<wgpu::RenderPassEncoder>(preEncoder.second).End();
            } else {
                std::any_cast<wgpu::ComputePassEncoder>(preEncoder.second).End();
            }
            encoderCreator(subpass);
        }
    }
    _activeSubpassIndex = 0;
    
    if (_gui) {
        ImDrawData *drawData = ImGui::GetDrawData();
        if (drawData) {
            if (preEncoder.first != Subpass::Type::Render) {
                std::any_cast<wgpu::ComputePassEncoder>(preEncoder.second).End();
                wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_desc);
                if (label) {
                    encoder.SetLabel(label.value().c_str());
                }
                preEncoder.first = Subpass::Type::Render;
                preEncoder.second = encoder;
            }
            auto encoder = std::any_cast<wgpu::RenderPassEncoder>(preEncoder.second);
            encoder.PushDebugGroup("GUI Rendering");
            _gui->draw(drawData, encoder);
            encoder.PopDebugGroup();
        }
    }
    
    if (preEncoder.first == Subpass::Type::Render) {
        std::any_cast<wgpu::RenderPassEncoder>(preEncoder.second).End();
    } else {
        std::any_cast<wgpu::ComputePassEncoder>(preEncoder.second).End();
    }
}

void RenderPass::addSubpass(std::unique_ptr<Subpass> &&subpass) {
    subpass->setRenderPass(this);
    _subpasses.emplace_back(std::move(subpass));
}

std::vector<std::unique_ptr<Subpass>> &RenderPass::subpasses() {
    return _subpasses;
}

std::unique_ptr<Subpass> &RenderPass::activeSubpass() {
    return _subpasses[_activeSubpassIndex];
}

//MARK: - ParentPass
void RenderPass::addParentPass(const std::string& name, RenderPass* pass) {
    auto iter = _parentPass.find(name);
    if (iter == _parentPass.end()) {
        _parentPass[name] = pass;
    } else {
        LOG(WARNING) << "already exists\n";
    }
}

RenderPass* RenderPass::removeParentPass(const std::string& name) {
    auto iter = _parentPass.find(name);
    if (iter != _parentPass.end()) {
        auto pass = iter->second;
        _parentPass.erase(iter);
        return pass;
    } else {
        LOG(WARNING) << "can't find pass\n";
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

void RenderPass::clearParentPass() {
    _parentPass.clear();
}

ResourceCache& RenderPass::resourceCache() {
    return _resourceCache;
}

}
