//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/debug_renderer_impl.h"

#include "vox.base/logging.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/scene.h"
#include "vox.render/shader/shader_common.h"

namespace vox::physics_debugger {
DebugRendererImpl::DebugRendererImpl(Entity *entity) : Script(entity) {}

void DebugRendererImpl::onAwake() {
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
    _line_material = std::make_shared<UnlitMaterial>(scene()->device());
    _line_buffer_mesh = MeshManager::GetSingleton().LoadBufferMesh();
    _line_buffer_mesh->setVertexLayouts(_line_layouts);
    auto lineRenderer = entity()->addComponent<MeshRenderer>();
    lineRenderer->setMesh(_line_buffer_mesh);
    lineRenderer->setMaterial(_line_material);

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

void DebugRendererImpl::onUpdate(float deltaTime) {
    DrawLines();
    DrawTriangles();
    DrawTexts();
}

void DebugRendererImpl::Clear() {
    ClearLines();
    ClearTriangles();
    ClearTexts();
}

void DebugRendererImpl::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {
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

void DebugRendererImpl::FinalizePrimitive() {}

void DebugRendererImpl::EnsurePrimitiveSpace(int inVtxSize) {}

//----------------------------------------------------------------------------------------------------------------------

DebugRendererImpl::Batch DebugRendererImpl::CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) {}

DebugRendererImpl::Batch DebugRendererImpl::CreateTriangleBatch(const Vertex *inVertices,
                                                                int inVertexCount,
                                                                const uint32 *inIndices,
                                                                int inIndexCount) {}

void DebugRendererImpl::DrawGeometry(Mat44Arg inModelMatrix,
                                     const AABox &inWorldSpaceBounds,
                                     float inLODScaleSq,
                                     ColorArg inModelColor,
                                     const GeometryRef &inGeometry,
                                     ECullMode inCullMode,
                                     ECastShadow inCastShadow,
                                     EDrawMode inDrawMode) {
    lock_guard lock(mPrimitivesLock);

    // Our pixel shader uses alpha only to turn on/off shadows
    ColorArg color = inCastShadow == ECastShadow::On ? ColorArg(inModelColor, 255) : ColorArg(inModelColor, 0);

    if (inDrawMode == EDrawMode::Wireframe) {
        mWireframePrimitives[inGeometry].mInstances.push_back(
                {inModelMatrix, inModelMatrix.GetDirectionPreservingMatrix(), color, inWorldSpaceBounds, inLODScaleSq});
        ++mNumInstances;
    } else {
        if (inCullMode != ECullMode::CullFrontFace) {
            mPrimitives[inGeometry].mInstances.push_back({inModelMatrix, inModelMatrix.GetDirectionPreservingMatrix(),
                                                          color, inWorldSpaceBounds, inLODScaleSq});
            ++mNumInstances;
        }
    }
}

void DebugRendererImpl::ClearMap(InstanceMap &ioInstances) {
    Array<GeometryRef> to_delete;

    for (InstanceMap::value_type &kv : ioInstances) {
        if (kv.second.mInstances.empty())
            to_delete.push_back(kv.first);
        else
            kv.second.mInstances.clear();
    }

    for (GeometryRef &b : to_delete) ioInstances.erase(b);
}

void DebugRendererImpl::ClearTriangles() {
    lock_guard lock(mPrimitivesLock);

    // Close any primitive that's being built
    FinalizePrimitive();

    // Move primitives to draw back to the free list
    ClearMap(mWireframePrimitives);
    ClearMap(mPrimitives);
    mTempPrimitives.clear();  // These are created by FinalizePrimitive() and need to be cleared every frame
    mNumInstances = 0;
}

void DebugRendererImpl::DrawTriangles() {}

void DebugRendererImpl::DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx) {}

//----------------------------------------------------------------------------------------------------------------------
void DebugRendererImpl::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {
    lock_guard lock(mLinesLock);
    mLines.push_back(Line(inFrom, inTo,
                          vox::Color(float(inColor.r) / 255.f, float(inColor.g) / 255.f, float(inColor.b) / 255.f,
                                     float(inColor.a) / 255.f)));
}

void DebugRendererImpl::DrawLines() {
    _line_buffer = std::make_unique<Buffer>(scene()->device(), mLines.data(), mLines.size() * sizeof(Line),
                                            wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);
    _line_buffer_mesh->clearSubMesh();
    _line_buffer_mesh->addSubMesh(0, mLines.size() * 2, wgpu::PrimitiveTopology::LineList);
    _line_buffer_mesh->setVertexBufferBinding(*_line_buffer, 0);
}

void DebugRendererImpl::ClearLines() {
    lock_guard lock(mLinesLock);
    mLines.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void DebugRendererImpl::DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) {
    lock_guard lock(mTextsLock);
    mTexts.emplace_back(inPosition, inString, inColor, inHeight);
}

void DebugRendererImpl::DrawTexts() {
    for (const auto &text : mTexts) {
        // todo: use ImGUI addText instead
        LOGI(text.mText)
    }
}

void DebugRendererImpl::ClearTexts() {
    lock_guard lock(mTextsLock);
    mTexts.clear();
}

}  // namespace vox::physics_debugger