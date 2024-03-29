//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_view/physics_view_subpass.h"

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/rendering/render_pass.h"

namespace vox::physics_view {
PhysicsViewSubpass::PhysicsViewSubpass(RenderContext *renderContext,
                                       wgpu::TextureFormat depthStencilTextureFormat,
                                       Scene *scene,
                                       Camera *camera)
    : Subpass(renderContext, scene, camera),
      _depthStencilTextureFormat(depthStencilTextureFormat),
      _vpMatrix(renderContext->device(), sizeof(Matrix4x4F), wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst) {}

void PhysicsViewSubpass::prepare() {
    {
        _line_attributes.resize(2);
        _line_attributes[0].shaderLocation = 0;
        _line_attributes[0].format = wgpu::VertexFormat::Float32x3;
        _line_attributes[0].offset = 0;
        _line_attributes[1].shaderLocation = 1;
        _line_attributes[1].format = wgpu::VertexFormat::Unorm8x4;
        _line_attributes[1].offset = 12;
        _line_layouts.resize(1);
        _line_layouts[0].attributeCount = _line_attributes.size();
        _line_layouts[0].attributes = _line_attributes.data();
        _line_layouts[0].stepMode = wgpu::VertexStepMode::Vertex;
        _line_layouts[0].arrayStride = 16;

        _vertex_attributes.resize(4);
        _vertex_attributes[0].shaderLocation = 0;
        _vertex_attributes[0].format = wgpu::VertexFormat::Float32x3;
        _vertex_attributes[0].offset = 0;
        _vertex_attributes[1].shaderLocation = 1;
        _vertex_attributes[1].format = wgpu::VertexFormat::Float32x3;
        _vertex_attributes[1].offset = 12;
        _vertex_attributes[2].shaderLocation = 2;
        _vertex_attributes[2].format = wgpu::VertexFormat::Float32x2;
        _vertex_attributes[2].offset = 24;
        _vertex_attributes[3].shaderLocation = 3;
        _vertex_attributes[3].format = wgpu::VertexFormat::Unorm8x4;
        _vertex_attributes[3].offset = 32;

        _instance_attributes.resize(9);
        _instance_attributes[0].shaderLocation = 4;
        _instance_attributes[0].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[0].offset = 0;
        _instance_attributes[1].shaderLocation = 5;
        _instance_attributes[1].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[1].offset = 16;
        _instance_attributes[2].shaderLocation = 6;
        _instance_attributes[2].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[2].offset = 32;
        _instance_attributes[3].shaderLocation = 7;
        _instance_attributes[3].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[3].offset = 48;
        _instance_attributes[4].shaderLocation = 8;
        _instance_attributes[4].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[4].offset = 64;
        _instance_attributes[5].shaderLocation = 9;
        _instance_attributes[5].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[5].offset = 80;
        _instance_attributes[6].shaderLocation = 10;
        _instance_attributes[6].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[6].offset = 96;
        _instance_attributes[7].shaderLocation = 11;
        _instance_attributes[7].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[7].offset = 112;
        _instance_attributes[8].shaderLocation = 12;
        _instance_attributes[8].format = wgpu::VertexFormat::Unorm8x4;
        _instance_attributes[8].offset = 128;

        _triangle_layouts.resize(2);
        _triangle_layouts[0].attributes = _vertex_attributes.data();
        _triangle_layouts[0].attributeCount = _vertex_attributes.size();
        _triangle_layouts[0].stepMode = wgpu::VertexStepMode::Vertex;
        _triangle_layouts[0].arrayStride = 36;
        _triangle_layouts[1].attributes = _instance_attributes.data();
        _triangle_layouts[1].attributeCount = _instance_attributes.size();
        _triangle_layouts[1].stepMode = wgpu::VertexStepMode::Instance;
        _triangle_layouts[1].arrayStride = 144;  // alignas(16)
    }

    _depthStencil.format = _depthStencilTextureFormat;
    _depthStencil.depthWriteEnabled = true;
    _depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;

    _colorTargetState.format = _renderContext->drawableTextureFormat();
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "Physics Debugger Pipeline";
    // BindGroupLayout
    {
        _bindGroupLayoutEntry.resize(1);
        _bindGroupLayoutEntry[0].binding = 10;
        _bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
        _bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
        _bindGroupLayoutDescriptor.entryCount = static_cast<uint32_t>(_bindGroupLayoutEntry.size());
        _bindGroupLayoutDescriptor.entries = _bindGroupLayoutEntry.data();
        _bindGroupLayout = ResourceCache::GetSingleton().requestBindGroupLayout(_bindGroupLayoutDescriptor);
    }
    // BindGroup
    {
        _bindGroupEntries.resize(1);
        _bindGroupEntries[0].binding = 10;
        _bindGroupEntries[0].size = sizeof(Matrix4x4F);
        _bindGroupEntries[0].buffer = _vpMatrix.handle();
        _bindGroupDescriptor.entryCount = static_cast<uint32_t>(_bindGroupEntries.size());
        _bindGroupDescriptor.entries = _bindGroupEntries.data();
        _bindGroupDescriptor.layout = _bindGroupLayout;
        _bindGroup = ResourceCache::GetSingleton().requestBindGroup(_bindGroupDescriptor);
    }
    // PipelineLayout
    {
        _pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        _pipelineLayoutDescriptor.bindGroupLayouts = &_bindGroupLayout;
        _pipelineLayout = ResourceCache::GetSingleton().requestPipelineLayout(_pipelineLayoutDescriptor);
        _forwardPipelineDescriptor.layout = _pipelineLayout;
    }
    // Line
    {
        ShaderVariant variant;
        _fragment.entryPoint = "main";
        _fragment.module = ResourceCache::GetSingleton()
                                   .requestShaderModule(wgpu::ShaderStage::Fragment,
                                                        ShaderSource("toolkit/physics_view/line.frag"), variant)
                                   .handle();
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module =
                ResourceCache::GetSingleton()
                        .requestShaderModule(wgpu::ShaderStage::Vertex, ShaderSource("toolkit/physics_view/line.vert"),
                                             variant)
                        .handle();

        _forwardPipelineDescriptor.primitive.frontFace = wgpu::FrontFace::CW;
        _forwardPipelineDescriptor.primitive.cullMode = wgpu::CullMode::Back;
        _forwardPipelineDescriptor.primitive.topology = wgpu::PrimitiveTopology::LineList;
        _forwardPipelineDescriptor.vertex.bufferCount = _line_layouts.size();
        _forwardPipelineDescriptor.vertex.buffers = _line_layouts.data();
        _linePipeline = ResourceCache::GetSingleton().requestPipeline(_forwardPipelineDescriptor);
    }

    // Triangle
    {
        ShaderVariant variant;
        _fragment.entryPoint = "main";
        _fragment.module = ResourceCache::GetSingleton()
                                   .requestShaderModule(wgpu::ShaderStage::Fragment,
                                                        ShaderSource("toolkit/physics_view/triangle.frag"), variant)
                                   .handle();
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module =
                ResourceCache::GetSingleton()
                        .requestShaderModule(wgpu::ShaderStage::Vertex,
                                             ShaderSource("toolkit/physics_view/triangle.vert"), variant)
                        .handle();

        _forwardPipelineDescriptor.primitive.frontFace = wgpu::FrontFace::CW;
        _forwardPipelineDescriptor.primitive.cullMode = wgpu::CullMode::None;
        _forwardPipelineDescriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
        _forwardPipelineDescriptor.vertex.bufferCount = _triangle_layouts.size();
        _forwardPipelineDescriptor.vertex.buffers = _triangle_layouts.data();
        _trianglePipeline = ResourceCache::GetSingleton().requestPipeline(_forwardPipelineDescriptor);
    }

    // Create instances buffer
    for (auto &n : mInstancesBuffer) {
        n = new RenderInstances(_scene->device());
    }

    // Create empty batch
    Vertex empty_vertex{Float3(0, 0, 0), Float3(1, 0, 0), Float2(0, 0), JPH::Color::sWhite};
    uint32 empty_indices[] = {0, 0, 0};
    mEmptyBatch = CreateTriangleBatch(&empty_vertex, 1, empty_indices, 3);

    DebugRenderer::Initialize();
}

void PhysicsViewSubpass::draw(wgpu::RenderPassEncoder &commandEncoder) {
    const auto projectionMatrix = _camera->projectionMatrix();
    auto viewMatrix = _camera->viewMatrix();
    auto _matrix = projectionMatrix * viewMatrix;
    std::vector<uint8_t> bytes = to_bytes(_matrix);
    _renderContext->device().GetQueue().WriteBuffer(_vpMatrix.handle(), 0, bytes.data(), sizeof(Matrix4x4F));
    commandEncoder.SetBindGroup(0, _bindGroup);

    DrawLines(commandEncoder);
    DrawTriangles(commandEncoder);
    DrawTexts(commandEncoder);
}

void PhysicsViewSubpass::Clear() {
    ClearLines();
    ClearTriangles();
    ClearTexts();
}

void PhysicsViewSubpass::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {
    lock_guard lock(mPrimitivesLock);

    EnsurePrimitiveSpace(3);

    // Set alpha to zero to tell our pixel shader to not cast shadows for this triangle
    // this is because our algorithm only renders shadows for backfacing triangles and this
    // triangle doesn't have one
    ColorArg color(inColor, 0);

    // Construct triangle
    new ((Triangle *)mLockedVertices) Triangle(inV1, inV2, inV3, color);
    mLockedVertices += 3;

    // Update bounding box
    mLockedPrimitiveBounds.Encapsulate(inV1);
    mLockedPrimitiveBounds.Encapsulate(inV2);
    mLockedPrimitiveBounds.Encapsulate(inV3);
}

void PhysicsViewSubpass::FinalizePrimitive() {
    JPH_PROFILE_FUNCTION();

    if (mLockedPrimitive != nullptr) {
        auto *primitive = static_cast<BatchImpl *>(mLockedPrimitive.GetPtr());

        // Unlock the primitive
        primitive->UnlockVertexBuffer();

        // Set number of indices to draw
        primitive->SetNumVtxToDraw(int(mLockedVertices - mLockedVerticesStart));

        // Add to draw list
        mTempPrimitives[new Geometry(mLockedPrimitive, mLockedPrimitiveBounds)].mInstances.push_back(
                {Mat44::sIdentity(), Mat44::sIdentity(), JPH::Color::sWhite, mLockedPrimitiveBounds, 1.0f});
        ++mNumInstances;

        // Clear pointers
        mLockedPrimitive = nullptr;
        mLockedVerticesStart = nullptr;
        mLockedVertices = nullptr;
        mLockedVerticesEnd = nullptr;
        mLockedPrimitiveBounds = AABox();
    }
}

void PhysicsViewSubpass::EnsurePrimitiveSpace(int inVtxSize) {
    const int cVertexBufferSize = 10240;

    if (mLockedPrimitive == nullptr || mLockedVerticesEnd - mLockedVertices < inVtxSize) {
        FinalizePrimitive();

        // Create new
        auto *primitive = new BatchImpl(_scene->device(), wgpu::PrimitiveTopology::TriangleList);
        primitive->CreateVertexBuffer(cVertexBufferSize, sizeof(Vertex));
        mLockedPrimitive = primitive;

        // Lock buffers
        mLockedVerticesStart = mLockedVertices = (Vertex *)primitive->LockVertexBuffer();
        mLockedVerticesEnd = mLockedVertices + cVertexBufferSize;
    }
}

//----------------------------------------------------------------------------------------------------------------------

PhysicsViewSubpass::Batch PhysicsViewSubpass::CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) {
    if (inTriangles == nullptr || inTriangleCount == 0) return mEmptyBatch;

    auto *primitive = new BatchImpl(_scene->device(), wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(3 * inTriangleCount, sizeof(Vertex), inTriangles);

    return primitive;
}

PhysicsViewSubpass::Batch PhysicsViewSubpass::CreateTriangleBatch(const Vertex *inVertices,
                                                                  int inVertexCount,
                                                                  const uint32 *inIndices,
                                                                  int inIndexCount) {
    if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr || inIndexCount == 0) return mEmptyBatch;

    auto *primitive = new BatchImpl(_scene->device(), wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(inVertexCount, sizeof(Vertex), inVertices);
    primitive->CreateIndexBuffer(inIndexCount, inIndices);

    return primitive;
}

void PhysicsViewSubpass::DrawGeometry(Mat44Arg inModelMatrix,
                                      const AABox &inWorldSpaceBounds,
                                      float inLODScaleSq,
                                      ColorArg inModelColor,
                                      const GeometryRef &inGeometry,
                                      ECullMode inCullMode,
                                      ECastShadow inCastShadow,
                                      EDrawMode inDrawMode) {
    lock_guard lock(mPrimitivesLock);

    mPrimitives[inGeometry].mInstances.push_back({inModelMatrix, inModelMatrix.GetDirectionPreservingMatrix(),
                                                  ColorArg(inModelColor, 255), inWorldSpaceBounds, inLODScaleSq});
    ++mNumInstances;
}

void PhysicsViewSubpass::ClearMap(InstanceMap &ioInstances) {
    Array<GeometryRef> to_delete;

    for (InstanceMap::value_type &kv : ioInstances) {
        if (kv.second.mInstances.empty())
            to_delete.push_back(kv.first);
        else
            kv.second.mInstances.clear();
    }

    for (GeometryRef &b : to_delete) ioInstances.erase(b);
}

void PhysicsViewSubpass::ClearTriangles() {
    lock_guard lock(mPrimitivesLock);

    // Close any primitive that's being built
    FinalizePrimitive();

    // Move primitives to draw back to the free list
    ClearMap(mWireframePrimitives);
    ClearMap(mPrimitives);
    mTempPrimitives.clear();  // These are created by FinalizePrimitive() and need to be cleared every frame
    mNumInstances = 0;
}

void PhysicsViewSubpass::DrawTriangles(wgpu::RenderPassEncoder &commandEncoder) {
    JPH_PROFILE_FUNCTION();

    lock_guard lock(mPrimitivesLock);

    // Finish the last primitive
    FinalizePrimitive();

    auto cameraPos = _camera->entity()->transform->worldPosition();
    Vec3 camera_pos = {cameraPos.x, cameraPos.y, cameraPos.z};

    // Resize instances buffer and copy all visible instance data into it
    if (mNumInstances > 0) {
        // Create instances buffer
        RenderInstances *instances_buffer = mInstancesBuffer[_currentFrameIndex];
        instances_buffer->CreateBuffer(2 * mNumInstances, sizeof(RenderInstances::Instance));
        auto *dst_instance = reinterpret_cast<RenderInstances::Instance *>(instances_buffer->Lock());

        // Next write index
        int dst_index = 0;

        // This keeps track of which instances use which lod, first array: 0 = geometry pass
        Array<Array<int>> lod_indices[1];

        for (InstanceMap *primitive_map : {&mPrimitives, &mTempPrimitives, &mWireframePrimitives})
            for (InstanceMap::value_type &v : *primitive_map) {
                // Get LODs
                const Array<LOD> &geometry_lods = v.first->mLODs;
                size_t num_lods = geometry_lods.size();
                JPH_ASSERT(num_lods > 0);

                // Ensure that our lod index array is big enough (to avoid reallocating memory too often)
                if (lod_indices[0].size() < num_lods) lod_indices[0].resize(num_lods);

                // Iterate over all instances
                const Array<InstanceWithLODInfo> &instances = v.second.mInstances;
                for (size_t i = 0; i < instances.size(); ++i) {
                    const InstanceWithLODInfo &src_instance = instances[i];

                    // Check if it overlaps with the camera frustum
                    // Figure out which LOD to use
                    float dist_sq = src_instance.mWorldSpaceBounds.GetSqDistanceTo(camera_pos);
                    for (size_t lod = 0; lod < num_lods; ++lod)
                        if (dist_sq <= src_instance.mLODScaleSq * Square(geometry_lods[lod].mDistance)) {
                            // Store which index goes in which LOD
                            lod_indices[0][lod].push_back((int)i);
                            break;
                        }
                }

                // Loop over both passes: 0 = geometry
                Array<int> *start_idx[] = {&v.second.mGeometryStartIdx};
                for (int type = 0; type < 1; ++type) {
                    // Reserve space for instance indices
                    Array<int> &type_start_idx = *start_idx[type];
                    type_start_idx.resize(num_lods + 1);

                    // Write out geometry pass instances
                    for (size_t lod = 0; lod < num_lods; ++lod) {
                        // Write start index for this LOD
                        type_start_idx[lod] = dst_index;

                        // Copy instances
                        Array<int> &this_lod_indices = lod_indices[type][lod];
                        for (int i : this_lod_indices) {
                            const RenderInstances::Instance &src_instance = instances[i];
                            dst_instance[dst_index++] = src_instance;
                        }

                        // Prepare for next iteration (will preserve memory)
                        this_lod_indices.clear();
                    }

                    // Write out end of last LOD
                    type_start_idx.back() = dst_index;
                }
            }

        instances_buffer->Unlock();
    }

    if (!mPrimitives.empty() || !mTempPrimitives.empty()) {
        // Bind the normal shader, back face culling
        commandEncoder.SetPipeline(_trianglePipeline);

        // Draw all primitives
        if (mNumInstances > 0)
            for (InstanceMap::value_type &v : mPrimitives)
                DrawInstances(commandEncoder, v.first, v.second.mGeometryStartIdx);
        for (InstanceMap::value_type &v : mTempPrimitives)
            DrawInstances(commandEncoder, v.first, v.second.mGeometryStartIdx);
    }

    if (!mWireframePrimitives.empty()) {
        // Wire frame mode
        commandEncoder.SetPipeline(_trianglePipeline);

        // Draw all wireframe primitives
        for (InstanceMap::value_type &v : mWireframePrimitives)
            DrawInstances(commandEncoder, v.first, v.second.mGeometryStartIdx);
    }
    _currentFrameIndex = 1 - _currentFrameIndex;
}

void PhysicsViewSubpass::DrawInstances(wgpu::RenderPassEncoder &commandEncoder,
                                       const Geometry *inGeometry,
                                       const Array<int> &inStartIdx) {
    RenderInstances *instances_buffer = mInstancesBuffer[_currentFrameIndex];

    if (!inStartIdx.empty()) {
        // Get LODs
        const Array<LOD> &geometry_lods = inGeometry->mLODs;

        // Write instances for all LODS
        int next_start_idx = inStartIdx.front();
        for (size_t lod = 0; lod < geometry_lods.size(); ++lod) {
            int start_idx = next_start_idx;
            next_start_idx = inStartIdx[lod + 1];
            int num_instances = next_start_idx - start_idx;
            instances_buffer->Draw(commandEncoder, static_cast<BatchImpl *>(geometry_lods[lod].mTriangleBatch.GetPtr()),
                                   start_idx, num_instances);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsViewSubpass::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {
    lock_guard lock(mLinesLock);
    mLines.push_back(Line(inFrom, inTo, inColor));
}

void PhysicsViewSubpass::DrawLines(wgpu::RenderPassEncoder &commandEncoder) {
    JPH_PROFILE_FUNCTION();

    lock_guard lock(mLinesLock);
    // Draw the lines
    if (!mLines.empty()) {
        RenderPrimitive primitive(_scene->device(), wgpu::PrimitiveTopology::LineList);
        primitive.CreateVertexBuffer((int)mLines.size() * 2, sizeof(Line) / 2);
        void *data = primitive.LockVertexBuffer();
        memcpy(data, &mLines[0], mLines.size() * sizeof(Line));
        primitive.UnlockVertexBuffer();
        commandEncoder.SetPipeline(_linePipeline);
        primitive.Draw(commandEncoder);
    }
}

void PhysicsViewSubpass::ClearLines() {
    lock_guard lock(mLinesLock);
    mLines.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsViewSubpass::DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) {
    lock_guard lock(mTextsLock);
    mTexts.emplace_back(inPosition, inString, inColor, inHeight);
}

void PhysicsViewSubpass::DrawTexts(wgpu::RenderPassEncoder &commandEncoder) {
    for (const auto &text : mTexts) {
        // todo: use ImGUI addText instead
        LOGI(text.mText)
    }
}

void PhysicsViewSubpass::ClearTexts() {
    lock_guard lock(mTextsLock);
    mTexts.clear();
}

}  // namespace vox::physics_view
