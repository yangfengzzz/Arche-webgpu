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

using namespace JPH;

namespace vox::physics_debugger {

class DebugRendererImp final : public DebugRenderer {
public:
    /// Implementation of DebugRenderer interface
    void DrawLine(const Float3 &inFrom, const Float3 &inTo, ColorArg inColor) override;
    void DrawTriangle(Vec3Arg inV1, Vec3Arg inV2, Vec3Arg inV3, ColorArg inColor) override;
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
    void DrawText3D(Vec3Arg inPosition, const string_view &inString, ColorArg inColor, float inHeight) override;
};
}  // namespace vox::physics_debugger