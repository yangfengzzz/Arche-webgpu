//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/shader/shader_common.h"

namespace vox::physics_debugger {
PhysicsDebugSubpass::PhysicsDebugSubpass(RenderContext *renderContext,
                                           wgpu::TextureFormat depthStencilTextureFormat,
                                           Scene *scene,
                                           Camera *camera)
    : Subpass(renderContext, scene, camera),
      _depthStencilTextureFormat(depthStencilTextureFormat),
      _vpMatrix(renderContext->device(), sizeof(Matrix4x4F), wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst) {}

void PhysicsDebugSubpass::prepare() {
    {
        _line_attributes.resize(2);
        _line_attributes[0].shaderLocation = (uint32_t)Attributes::POSITION;
        _line_attributes[0].format = wgpu::VertexFormat::Float32x3;
        _line_attributes[0].offset = 0;
        _line_attributes[1].shaderLocation = (uint32_t)Attributes::COLOR_0;
        _line_attributes[1].format = wgpu::VertexFormat::Float32x4;
        _line_attributes[1].offset = 12;
        _line_layouts.resize(1);
        _line_layouts[0].attributeCount = 2;
        _line_layouts[0].attributes = _line_attributes.data();
        _line_layouts[0].stepMode = wgpu::VertexStepMode::Vertex;
        _line_layouts[0].arrayStride = 28;

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
        _vertex_attributes[3].format = wgpu::VertexFormat::Uint8x4;
        _vertex_attributes[3].offset = 32;

        _instance_attributes.resize(9);
        _instance_attributes[0].shaderLocation = 0;
        _instance_attributes[0].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[0].offset = 0;
        _instance_attributes[1].shaderLocation = 1;
        _instance_attributes[1].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[1].offset = 16;
        _instance_attributes[2].shaderLocation = 2;
        _instance_attributes[2].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[2].offset = 32;
        _instance_attributes[3].shaderLocation = 3;
        _instance_attributes[3].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[3].offset = 48;
        _instance_attributes[4].shaderLocation = 4;
        _instance_attributes[4].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[4].offset = 64;
        _instance_attributes[5].shaderLocation = 5;
        _instance_attributes[5].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[5].offset = 80;
        _instance_attributes[6].shaderLocation = 6;
        _instance_attributes[6].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[6].offset = 96;
        _instance_attributes[7].shaderLocation = 7;
        _instance_attributes[7].format = wgpu::VertexFormat::Float32x4;
        _instance_attributes[7].offset = 112;
        _instance_attributes[8].shaderLocation = 8;
        _instance_attributes[8].format = wgpu::VertexFormat::Uint8x4;
        _instance_attributes[8].offset = 128;

        _triangle_layouts.resize(2);
        _triangle_layouts[0].attributes = _vertex_attributes.data();
        _triangle_layouts[0].attributeCount = _vertex_attributes.size();
        _triangle_layouts[0].stepMode = wgpu::VertexStepMode::Vertex;
        _triangle_layouts[0].arrayStride = 36;
        _triangle_layouts[1].attributes = _instance_attributes.data();
        _triangle_layouts[1].attributeCount = _instance_attributes.size();
        _triangle_layouts[1].stepMode = wgpu::VertexStepMode::Instance;
        _triangle_layouts[1].arrayStride = 132;
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
        _bindGroupEntries.resize(3);
        _bindGroupEntries[0].binding = 10;
        _bindGroupEntries[0].size = sizeof(Matrix4x4F);
        _bindGroupEntries[0].buffer = _vpMatrix.handle();
        _bindGroupDescriptor.entryCount = static_cast<uint32_t>(_bindGroupEntries.size());
        _bindGroupDescriptor.entries = _bindGroupEntries.data();
        _bindGroupDescriptor.layout = _bindGroupLayout;
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
                                   .requestShaderModule(wgpu::ShaderStage::Fragment, ShaderSource("toolkit/physics_debugger/line.vert"), variant)
                                   .handle();
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module =
                ResourceCache::GetSingleton()
                        .requestShaderModule(wgpu::ShaderStage::Vertex, ShaderSource("toolkit/physics_debugger/line.frag"), variant)
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
                                   .requestShaderModule(wgpu::ShaderStage::Fragment, ShaderSource("toolkit/physics_debugger/triangle.vert"), variant)
                                   .handle();
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module =
                ResourceCache::GetSingleton()
                        .requestShaderModule(wgpu::ShaderStage::Vertex, ShaderSource("toolkit/physics_debugger/triangle.frag"), variant)
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

void PhysicsDebugSubpass::draw(wgpu::RenderPassEncoder &commandEncoder) {
    const auto projectionMatrix = _camera->projectionMatrix();
    auto viewMatrix = _camera->viewMatrix();
    auto _matrix = projectionMatrix * viewMatrix;
    std::vector<uint8_t> bytes = to_bytes(_matrix);
    _renderContext->device().GetQueue().WriteBuffer(_vpMatrix.handle(), 0, bytes.data(), sizeof(Matrix4x4F));

    DrawLines(commandEncoder);
    DrawTriangles(commandEncoder);
    DrawTexts(commandEncoder);
}

void PhysicsDebugSubpass::Clear() {
    ClearLines();
    ClearTriangles();
    ClearTexts();
}

void PhysicsDebugSubpass::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {
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

void PhysicsDebugSubpass::FinalizePrimitive() {
    JPH_PROFILE_FUNCTION()

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

void PhysicsDebugSubpass::EnsurePrimitiveSpace(int inVtxSize) {
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

PhysicsDebugSubpass::Batch PhysicsDebugSubpass::CreateTriangleBatch(const Triangle *inTriangles,
                                                                      int inTriangleCount) {
    if (inTriangles == nullptr || inTriangleCount == 0) return mEmptyBatch;

    auto *primitive = new BatchImpl(_scene->device(), wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(3 * inTriangleCount, sizeof(Vertex), inTriangles);

    return primitive;
}

PhysicsDebugSubpass::Batch PhysicsDebugSubpass::CreateTriangleBatch(const Vertex *inVertices,
                                                                      int inVertexCount,
                                                                      const uint32 *inIndices,
                                                                      int inIndexCount) {
    if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr || inIndexCount == 0) return mEmptyBatch;

    auto *primitive = new BatchImpl(_scene->device(), wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(inVertexCount, sizeof(Vertex), inVertices);
    primitive->CreateIndexBuffer(inIndexCount, inIndices);

    return primitive;
}

void PhysicsDebugSubpass::DrawGeometry(Mat44Arg inModelMatrix,
                                        const AABox &inWorldSpaceBounds,
                                        float inLODScaleSq,
                                        ColorArg inModelColor,
                                        const GeometryRef &inGeometry,
                                        ECullMode inCullMode,
                                        ECastShadow inCastShadow,
                                        EDrawMode inDrawMode) {
    lock_guard lock(mPrimitivesLock);

    if (inDrawMode == EDrawMode::Wireframe) {
        mWireframePrimitives[inGeometry].mInstances.push_back(
                {inModelMatrix, inModelMatrix.GetDirectionPreservingMatrix(), ColorArg(inModelColor, 255),
                 inWorldSpaceBounds, inLODScaleSq});
        ++mNumInstances;
    } else {
        if (inCullMode != ECullMode::CullFrontFace) {
            mPrimitives[inGeometry].mInstances.push_back({inModelMatrix, inModelMatrix.GetDirectionPreservingMatrix(),
                                                          ColorArg(inModelColor, 255), inWorldSpaceBounds,
                                                          inLODScaleSq});
            ++mNumInstances;
        }
    }
}

void PhysicsDebugSubpass::ClearMap(InstanceMap &ioInstances) {
    Array<GeometryRef> to_delete;

    for (InstanceMap::value_type &kv : ioInstances) {
        if (kv.second.mInstances.empty())
            to_delete.push_back(kv.first);
        else
            kv.second.mInstances.clear();
    }

    for (GeometryRef &b : to_delete) ioInstances.erase(b);
}

void PhysicsDebugSubpass::ClearTriangles() {
    lock_guard lock(mPrimitivesLock);

    // Close any primitive that's being built
    FinalizePrimitive();

    // Move primitives to draw back to the free list
    ClearMap(mWireframePrimitives);
    ClearMap(mPrimitives);
    mTempPrimitives.clear();  // These are created by FinalizePrimitive() and need to be cleared every frame
    mNumInstances = 0;
}

void PhysicsDebugSubpass::DrawTriangles(wgpu::RenderPassEncoder &commandEncoder) {
    JPH_PROFILE_FUNCTION()

    lock_guard lock(mPrimitivesLock);

    // Finish the last primitive
    FinalizePrimitive();

    auto cameraPos = _camera->entity()->transform->worldPosition();
    Vec3 camera_pos = {cameraPos.x, cameraPos.y, cameraPos.z};

    // Resize instances buffer and copy all visible instance data into it
    if (mNumInstances > 0) {
        // Create instances buffer
        RenderInstances *instances_buffer = mInstancesBuffer[_currentFrameIndex];
        instances_buffer->CreateBuffer(2 * mNumInstances, sizeof(Instance));
        auto *dst_instance = reinterpret_cast<Instance *>(instances_buffer->Lock());

        // Next write index
        int dst_index = 0;

        // This keeps track of which instances use which lod, first array: 0 = light pass, 1 = geometry pass
        Array<Array<int>> lod_indices[2];

        for (InstanceMap *primitive_map : {&mPrimitives, &mTempPrimitives, &mWireframePrimitives})
            for (InstanceMap::value_type &v : *primitive_map) {
                // Get LODs
                const Array<LOD> &geometry_lods = v.first->mLODs;
                size_t num_lods = geometry_lods.size();
                JPH_ASSERT(num_lods > 0);

                // Ensure that our lod index array is big enough (to avoid reallocating memory too often)
                if (lod_indices[0].size() < num_lods) lod_indices[0].resize(num_lods);
                if (lod_indices[1].size() < num_lods) lod_indices[1].resize(num_lods);

                // Iterate over all instances
                const Array<InstanceWithLODInfo> &instances = v.second.mInstances;
                for (size_t i = 0; i < instances.size(); ++i) {
                    const InstanceWithLODInfo &src_instance = instances[i];

                    // Check if it overlaps with the light or camera frustum
                    // Figure out which LOD to use
                    float dist_sq = src_instance.mWorldSpaceBounds.GetSqDistanceTo(camera_pos);
                    for (size_t lod = 0; lod < num_lods; ++lod)
                        if (dist_sq <= src_instance.mLODScaleSq * Square(geometry_lods[lod].mDistance)) {
                            // Store which index goes in which LOD
                            lod_indices[0][lod].push_back((int)i);
                            lod_indices[1][lod].push_back((int)i);
                            break;
                        }
                }

                // Loop over both passes: 0 = light, 1 = geometry
                Array<int> *start_idx[] = {&v.second.mLightStartIdx, &v.second.mGeometryStartIdx};
                for (int type = 0; type < 2; ++type) {
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
                            const Instance &src_instance = instances[i];
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
            for (InstanceMap::value_type &v : mPrimitives) DrawInstances(v.first, v.second.mGeometryStartIdx);
        for (InstanceMap::value_type &v : mTempPrimitives) DrawInstances(v.first, v.second.mGeometryStartIdx);
    }

    if (!mWireframePrimitives.empty()) {
        // Wire frame mode
        commandEncoder.SetPipeline(_trianglePipeline);

        // Draw all wireframe primitives
        for (InstanceMap::value_type &v : mWireframePrimitives) DrawInstances(v.first, v.second.mGeometryStartIdx);
    }
    _currentFrameIndex = 1 - _currentFrameIndex;
}

void PhysicsDebugSubpass::DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx) {}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsDebugSubpass::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {
    lock_guard lock(mLinesLock);
    mLines.push_back(Line(inFrom, inTo,
                          vox::Color(float(inColor.r) / 255.f, float(inColor.g) / 255.f, float(inColor.b) / 255.f,
                                     float(inColor.a) / 255.f)));
}

void PhysicsDebugSubpass::DrawLines(wgpu::RenderPassEncoder &commandEncoder) {
    JPH_PROFILE_FUNCTION()

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

void PhysicsDebugSubpass::ClearLines() {
    lock_guard lock(mLinesLock);
    mLines.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsDebugSubpass::DrawText3D(Vec3Arg inPosition,
                                      const string_view &inString,
                                      ColorArg inColor,
                                      float inHeight) {
    lock_guard lock(mTextsLock);
    mTexts.emplace_back(inPosition, inString, inColor, inHeight);
}

void PhysicsDebugSubpass::DrawTexts(wgpu::RenderPassEncoder &commandEncoder) {
    for (const auto &text : mTexts) {
        // todo: use ImGUI addText instead
        LOGI(text.mText)
    }
}

void PhysicsDebugSubpass::ClearTexts() {
    lock_guard lock(mTextsLock);
    mTexts.clear();
}

}  // namespace vox::physics_debugger