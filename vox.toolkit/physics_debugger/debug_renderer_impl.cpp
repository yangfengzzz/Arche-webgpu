//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/debug_renderer_impl.h"

namespace vox::physics_debugger {
void DebugRendererImpl::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {
    lock_guard lock(mLinesLock);
    mLines.push_back(Line(inFrom, inTo, inColor));
}

void DebugRendererImpl::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {}

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
                                    EDrawMode inDrawMode) {}

void DebugRendererImpl::DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) {}

void DebugRendererImpl::Draw() {}

void DebugRendererImpl::Clear() {}

void DebugRendererImpl::DrawTriangles() {}

void DebugRendererImpl::ClearTriangles() {}

void DebugRendererImpl::FinalizePrimitive() {}

void DebugRendererImpl::EnsurePrimitiveSpace(int inVtxSize) {}

void DebugRendererImpl::ClearMap(InstanceMap &ioInstances) {}

void DebugRendererImpl::DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx) {}

void DebugRendererImpl::DrawLines() {}

void DebugRendererImpl::ClearLines() {
    lock_guard lock(mLinesLock);
    mLines.clear();
}

void DebugRendererImpl::DrawTexts() {}
void DebugRendererImpl::ClearTexts() {}

}  // namespace vox::physics_debugger