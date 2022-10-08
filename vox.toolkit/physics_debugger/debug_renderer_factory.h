//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Jolt/Jolt.h>
#ifdef JPH_DEBUG_RENDERER
#include <Jolt/Renderer/DebugRenderer.h>
#else
// Hack to still compile DebugRenderer inside the test framework when Jolt is compiled without
#define JPH_DEBUG_RENDERER
#include <Jolt/Renderer/DebugRenderer.h>
#undef JPH_DEBUG_RENDERER
#endif
#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>

#include <utility>
#include <vector>

#include "vox.render/material/base_material.h"
#include "vox.render/rendering/subpass.h"
#include "vox.toolkit/physics_debugger/render_instances.h"

using namespace JPH;

namespace vox::physics_debugger {

class DebugRendererFactory final : public DebugRenderer, public Subpass {
public:
    DebugRendererFactory(RenderContext *renderContext,
                         wgpu::TextureFormat depthStencilTextureFormat,
                         Scene *scene,
                         Camera *camera);

    /// Implementation of DebugRenderer interface
    void DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) override;

    void DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) override;

    void DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) override;

public:
    Batch CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) override;

    Batch CreateTriangleBatch(const Vertex *inVertices,
                              int inVertexCount,
                              const uint32 *inIndices,
                              int inIndexCount) override;
    void DrawGeometry(Mat44Arg inModelMatrix,
                      const AABox &inWorldSpaceBounds,
                      float inLODScaleSq,
                      ColorArg inModelColor,
                      const GeometryRef &inGeometry,
                      ECullMode inCullMode,
                      ECastShadow inCastShadow,
                      EDrawMode inDrawMode) override;

    void draw(wgpu::RenderPassEncoder &commandEncoder) override;

    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    void prepare() override;

    /// Clear all primitives (to be called after drawing)
    void Clear();

private:
    void DrawTriangles(wgpu::RenderPassEncoder &commandEncoder);

    void ClearTriangles();

    /// Implementation specific batch object
    class BatchImpl : public RefTargetVirtual, public RenderPrimitive {
    public:
        JPH_OVERRIDE_NEW_DELETE

        BatchImpl(wgpu::Device &device, wgpu::PrimitiveTopology inType) : RenderPrimitive(device, inType) {}

        void AddRef() override { RenderPrimitive::AddRef(); }
        void Release() override {
            if (--mRefCount == 0) delete this;
        }
    };

    /// Properties for a single rendered instance
    struct Instance {
        /// Constructor
        Instance(Mat44Arg inModelMatrix, Mat44Arg inModelMatrixInvTrans, ColorArg inModelColor)
            : mModelMatrix(inModelMatrix), mModelMatrixInvTrans(inModelMatrixInvTrans), mModelColor(inModelColor) {}

        Mat44 mModelMatrix;
        Mat44 mModelMatrixInvTrans;
        ColorArg mModelColor;
    };

    /// Rendered instance with added information for lodding
    struct InstanceWithLODInfo : public Instance {
        /// Constructor
        InstanceWithLODInfo(Mat44Arg inModelMatrix,
                            Mat44Arg inModelMatrixInvTrans,
                            ColorArg inModelColor,
                            const AABox &inWorldSpaceBounds,
                            float inLODScaleSq)
            : Instance(inModelMatrix, inModelMatrixInvTrans, inModelColor),
              mWorldSpaceBounds(inWorldSpaceBounds),
              mLODScaleSq(inLODScaleSq) {}

        /// Bounding box for culling
        AABox mWorldSpaceBounds;

        /// Square of scale factor for LODding (1 = original, > 1 = lod out further, < 1 = lod out earlier)
        float mLODScaleSq;
    };

    /// Properties for a batch of instances that have the same primitive
    struct Instances {
        Array<InstanceWithLODInfo> mInstances;

        /// Start index in mInstancesBuffer for each of the LOD in the geometry pass. Length is one longer than the
        /// number of LODs to indicate how many instances the last lod has.
        Array<int> mGeometryStartIdx;

        /// Start index in mInstancesBuffer for each of the LOD in the light pass. Length is one longer than the number
        /// of LODs to indicate how many instances the last lod has.
        Array<int> mLightStartIdx;
    };

    using InstanceMap = UnorderedMap<GeometryRef, Instances>;

    /// List of primitives that are finished and ready for drawing
    InstanceMap mWireframePrimitives;
    InstanceMap mPrimitives;
    InstanceMap mTempPrimitives;
    int mNumInstances = 0;
    Ref<RenderInstances> mInstancesBuffer[2];

    /// Lock that protects the triangle batches from being accessed from multiple threads
    Mutex mPrimitivesLock;

    Batch mEmptyBatch;

    /// Primitive that is being built + its properties
    Batch mLockedPrimitive;
    Vertex *mLockedVerticesStart = nullptr;
    Vertex *mLockedVertices = nullptr;
    Vertex *mLockedVerticesEnd = nullptr;
    AABox mLockedPrimitiveBounds;

    /// Finalize the current locked primitive and add it to the primitives to draw
    void FinalizePrimitive();

    /// Ensure that the current locked primitive has space for a primitive consisting inVtxSize vertices
    void EnsurePrimitiveSpace(int inVtxSize);

    /// Clear map of instances and make it ready for the next frame
    static void ClearMap(InstanceMap &ioInstances);

    /// Helper function to draw instances
    inline void DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx);

private:
    void DrawLines(wgpu::RenderPassEncoder &commandEncoder);

    void ClearLines();

    /// A single line segment
    struct Line {
        Line(const Float3 &inFrom, const Float3 &inTo, Color inColor)
            : mFrom(inFrom), mFromColor(inColor), mTo(inTo), mToColor(inColor) {}

        Float3 mFrom;
        Color mFromColor;
        Float3 mTo;
        Color mToColor;
    };
    /// The list of line segments
    Array<Line> mLines;
    Mutex mLinesLock;

private:
    void DrawTexts(wgpu::RenderPassEncoder &commandEncoder);
    void ClearTexts();

    /// A single text string
    struct Text {
        Text(Vec3Arg inPosition, const string_view &inText, ColorArg inColor, float inHeight)
            : mPosition(inPosition), mText(inText), mColor(inColor), mHeight(inHeight) {}

        Vec3 mPosition;
        String mText;
        JPH::Color mColor;
        float mHeight;
    };

    /// All text strings that are to be drawn on screen
    Array<Text> mTexts;
    Mutex mTextsLock;

private:
    std::vector<wgpu::VertexAttribute> _vertex_attributes;
    std::vector<wgpu::VertexAttribute> _instance_attributes;
    std::vector<wgpu::VertexBufferLayout> _triangle_layouts;

    std::vector<wgpu::VertexAttribute> _line_attributes;
    std::vector<wgpu::VertexBufferLayout> _line_layouts;

    Buffer _vpMatrix;
    wgpu::TextureFormat _depthStencilTextureFormat;

    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::DepthStencilState _depthStencil;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;

    std::vector<wgpu::BindGroupLayoutEntry> _bindGroupLayoutEntry;
    wgpu::BindGroupLayoutDescriptor _bindGroupLayoutDescriptor;
    wgpu::BindGroupLayout _bindGroupLayout;

    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    wgpu::BindGroupDescriptor _bindGroupDescriptor;

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;

    wgpu::RenderPipeline _linePipeline;
    wgpu::RenderPipeline _trianglePipeline;
    int _currentFrameIndex = 0;
};
}  // namespace vox::physics_debugger