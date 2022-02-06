//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui.h"
#include "gui/imgui_impl_wgpu.h"

namespace vox {
GUI::GUI(wgpu::Device& device) {
    ImGui_ImplWGPU_Init(device.Get(), 3, WGPUTextureFormat_RGBA8UnormSrgb);
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
