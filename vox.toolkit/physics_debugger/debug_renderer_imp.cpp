//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/physics_debugger/debug_renderer_imp.h"

namespace vox::physics_debugger {
void DebugRendererImp::DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) {}

void DebugRendererImp::DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) {}

DebugRendererImp::Batch DebugRendererImp::CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) {}

DebugRendererImp::Batch DebugRendererImp::CreateTriangleBatch(const Vertex *inVertices,
                                                              int inVertexCount,
                                                              const uint32 *inIndices,
                                                              int inIndexCount) {}

void DebugRendererImp::DrawGeometry(Mat44Arg inModelMatrix,
                                    const AABox &inWorldSpaceBounds,
                                    float inLODScaleSq,
                                    ColorArg inModelColor,
                                    const GeometryRef &inGeometry,
                                    ECullMode inCullMode,
                                    ECastShadow inCastShadow,
                                    EDrawMode inDrawMode) {}

void DebugRendererImp::DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) {}

}  // namespace vox::physics_debugger