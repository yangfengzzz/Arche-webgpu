//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui.h"
#include "gui/imgui_impl_wgpu.h"

namespace vox {
GUI::GUI(RenderContext* context) {
    ImGui_ImplWGPU_Init(context->device().Get(), 3,
                        (WGPUTextureFormat)context->drawableTextureFormat(),
                        (WGPUTextureFormat)context->depthStencilTextureFormat());
}

GUI::~GUI() {
    ImGui_ImplWGPU_Shutdown();
}

void GUI::newFrame() {
    ImGui_ImplWGPU_NewFrame();
}

void GUI::draw(ImDrawData* drawData,
               wgpu::RenderPassEncoder& passEncoder) {
    ImGui_ImplWGPU_RenderDrawData(drawData, passEncoder.Get());
}

}
