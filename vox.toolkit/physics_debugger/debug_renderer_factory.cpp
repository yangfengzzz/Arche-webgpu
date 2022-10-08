//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/debug_renderer_factory.h"

#include "vox.base/logging.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_common.h"

namespace vox::physics_debugger {
DebugRendererFactory::DebugRendererFactory(Entity *entity) : _entity(entity) {
    DebugRenderer::Initialize();
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
    _line_material = std::make_shared<UnlitMaterial>(entity->scene()->device());
    auto line_buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
    line_buffer_mesh->setVertexLayouts(_line_layouts);
    auto lineRenderer = entity->addComponent<MeshRenderer>();
    lineRenderer->setMesh(line_buffer_mesh);
    lineRenderer->setMaterial(_line_material);
    _line_primitive = new RenderPrimitive(line_buffer_mesh, wgpu::PrimitiveTopology::LineList);

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

    // Create instances buffer
    for (auto &n : mInstancesBuffer) {
        auto buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
        buffer_mesh->setVertexLayouts(_triangle_layouts);
        auto renderer = _entity->addComponent<MeshRenderer>();
        renderer->setMesh(buffer_mesh);
        renderer->setMaterial(_triangle_material);
        n = new RenderInstances(buffer_mesh);
    }

    // Create empty batch
    Vertex empty_vertex{Float3(0, 0, 0), Float3(1, 0, 0), Float2(0, 0), JPH::Color::sWhite};
    uint32 empty_indices[] = {0, 0, 0};
    mEmptyBatch = CreateTriangleBatch(&empty_vertex, 1, empty_indices, 3);
}

void DebugRendererFactory::Draw() {
    DrawLines();
    DrawTriangles();
    DrawTexts();
}

void DebugRendererFactory::Clear() {
    ClearLines();
    ClearTriangles();
    ClearTexts();
}

void DebugRendererFactory::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {
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

void DebugRendererFactory::FinalizePrimitive() {
    JPH_PROFILE_FUNCTION()

    if (mLockedPrimitive != nullptr) {
        auto *primitive = static_cast<BatchImpl *>(mLockedPrimitive.GetPtr());

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

void DebugRendererFactory::EnsurePrimitiveSpace(int inVtxSize) {
    const int cVertexBufferSize = 10240;

    if (mLockedPrimitive == nullptr || mLockedVerticesEnd - mLockedVertices < inVtxSize) {
        FinalizePrimitive();

        // Create new
        auto buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
        buffer_mesh->setVertexLayouts(_triangle_layouts);
        auto renderer = _entity->addComponent<MeshRenderer>();
        renderer->setMesh(buffer_mesh);
        renderer->setMaterial(_triangle_material);
        auto *primitive = new BatchImpl(buffer_mesh, wgpu::PrimitiveTopology::TriangleList);
        primitive->CreateVertexBuffer(cVertexBufferSize, sizeof(Vertex));
        mLockedPrimitive = primitive;

        // Lock buffers
        // mLockedVerticesStart = mLockedVertices = (Vertex *)primitive->LockVertexBuffer();
        // mLockedVerticesEnd = mLockedVertices + cVertexBufferSize;
    }
}

//----------------------------------------------------------------------------------------------------------------------

DebugRendererFactory::Batch DebugRendererFactory::CreateTriangleBatch(const Triangle *inTriangles,
                                                                      int inTriangleCount) {
    if (inTriangles == nullptr || inTriangleCount == 0) return mEmptyBatch;

    auto buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
    buffer_mesh->setVertexLayouts(_triangle_layouts);
    auto renderer = _entity->addComponent<MeshRenderer>();
    renderer->setMesh(buffer_mesh);
    renderer->setMaterial(_triangle_material);
    auto *primitive = new BatchImpl(buffer_mesh, wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(3 * inTriangleCount, sizeof(Vertex), inTriangles);

    return primitive;
}

DebugRendererFactory::Batch DebugRendererFactory::CreateTriangleBatch(const Vertex *inVertices,
                                                                      int inVertexCount,
                                                                      const uint32 *inIndices,
                                                                      int inIndexCount) {
    if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr || inIndexCount == 0) return mEmptyBatch;

    auto buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
    buffer_mesh->setVertexLayouts(_triangle_layouts);
    auto renderer = _entity->addComponent<MeshRenderer>();
    renderer->setMesh(buffer_mesh);
    renderer->setMaterial(_triangle_material);
    auto *primitive = new BatchImpl(buffer_mesh, wgpu::PrimitiveTopology::TriangleList);
    primitive->CreateVertexBuffer(inVertexCount, sizeof(Vertex), inVertices);
    primitive->CreateIndexBuffer(inIndexCount, inIndices);

    return primitive;
}

void DebugRendererFactory::DrawGeometry(Mat44Arg inModelMatrix,
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

void DebugRendererFactory::ClearMap(InstanceMap &ioInstances) {
    Array<GeometryRef> to_delete;

    for (InstanceMap::value_type &kv : ioInstances) {
        if (kv.second.mInstances.empty())
            to_delete.push_back(kv.first);
        else
            kv.second.mInstances.clear();
    }

    for (GeometryRef &b : to_delete) ioInstances.erase(b);
}

void DebugRendererFactory::ClearTriangles() {
    lock_guard lock(mPrimitivesLock);

    // Close any primitive that's being built
    FinalizePrimitive();

    // Move primitives to draw back to the free list
    ClearMap(mWireframePrimitives);
    ClearMap(mPrimitives);
    mTempPrimitives.clear();  // These are created by FinalizePrimitive() and need to be cleared every frame
    mNumInstances = 0;
}

void DebugRendererFactory::DrawTriangles() {}

void DebugRendererFactory::DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx) {}

//----------------------------------------------------------------------------------------------------------------------
void DebugRendererFactory::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {
    lock_guard lock(mLinesLock);
    mLines.push_back(Line(inFrom, inTo,
                          vox::Color(float(inColor.r) / 255.f, float(inColor.g) / 255.f, float(inColor.b) / 255.f,
                                     float(inColor.a) / 255.f)));
}

void DebugRendererFactory::DrawLines() {
    JPH_PROFILE_FUNCTION()

    lock_guard lock(mLinesLock);
    // Draw the lines
    if (!mLines.empty()) {
        _line_primitive->CreateVertexBuffer((int)mLines.size() * 2, sizeof(Line) / 2, mLines.data());
    }
}

void DebugRendererFactory::ClearLines() {
    lock_guard lock(mLinesLock);
    mLines.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void DebugRendererFactory::DrawText3D(Vec3Arg inPosition,
                                      const string_view &inString,
                                      ColorArg inColor,
                                      float inHeight) {
    lock_guard lock(mTextsLock);
    mTexts.emplace_back(inPosition, inString, inColor, inHeight);
}

void DebugRendererFactory::DrawTexts() {
    for (const auto &text : mTexts) {
        // todo: use ImGUI addText instead
        LOGI(text.mText)
    }
}

void DebugRendererFactory::ClearTexts() {
    lock_guard lock(mTextsLock);
    mTexts.clear();
}

}  // namespace vox::physics_debugger