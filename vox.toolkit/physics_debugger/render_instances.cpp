//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/render_instances.h"

namespace vox::physics_debugger {
void RenderInstances::Clear() {}

void RenderInstances::CreateBuffer(int inNumInstances, int inInstanceSize) {}
void RenderInstances::UpdateBuffer() {}

void RenderInstances::Draw(RenderPrimitive *inPrimitive, int inStartInstance, int inNumInstances) const {}

}  // namespace vox::physics_debugger