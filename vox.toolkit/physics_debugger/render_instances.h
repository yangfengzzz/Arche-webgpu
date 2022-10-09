//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
//
#include <Jolt/Core/Color.h>

#include <vector>

#include "vox.toolkit/physics_debugger/render_primitive.h"

namespace vox::physics_debugger {

using namespace JPH;

/// Buffer that holds a list of instances (usually model transform etc.) for instance based rendering
class RenderInstances : public RefTarget<RenderInstances> {
public:
    /// Properties for a single rendered instance
    struct Instance {
        /// Constructor
        explicit Instance(Mat44Arg inModelMatrix = Mat44(),
                          Mat44Arg inModelMatrixInvTrans = Mat44(),
                          ColorArg inModelColor = JPH::Color())
            : mModelMatrix(inModelMatrix), mModelMatrixInvTrans(inModelMatrixInvTrans), mModelColor(inModelColor) {}

        Mat44 mModelMatrix;
        Mat44 mModelMatrixInvTrans;
        ColorArg mModelColor;
    };

    /// Constructor
    explicit RenderInstances(wgpu::Device& device) : device(device) {}
    ~RenderInstances() { Clear(); }

    /// Erase all instance data
    void Clear();

    /// Instance buffer management functions
    void CreateBuffer(int inNumInstances, int inInstanceSize);
    void* Lock();
    void Unlock();

    /// Draw the instances when context has been set by Renderer::BindShader
    void Draw(wgpu::RenderPassEncoder& passEncoder,
              RenderPrimitive* inPrimitive,
              int inStartInstance,
              int inNumInstances) const;

private:
    wgpu::Device& device;

    std::vector<Instance> instance_mapped_resource{};
    wgpu::Buffer mInstanceBuffer;
    int mInstanceBufferSize = 0;
    int mInstanceSize = 0;
};
}  // namespace vox::physics_debugger