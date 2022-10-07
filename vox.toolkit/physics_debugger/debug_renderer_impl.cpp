//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/debug_renderer_impl.h"

#include "vox.base/logging.h"

namespace vox::physics_debugger {
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
    Color color(inColor, 0);

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
    Color color = inCastShadow == ECastShadow::On ? Color(inModelColor, 255) : Color(inModelColor, 0);

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
    mLines.push_back(Line(inFrom, inTo, inColor));
}

void DebugRendererImpl::DrawLines() {}

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
        LOGI(text.mText);
    }
}

void DebugRendererImpl::ClearTexts() {
    lock_guard lock(mTextsLock);
    mTexts.clear();
}

}  // namespace vox::physics_debugger