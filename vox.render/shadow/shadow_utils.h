//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include "vox.math/bounding_frustum.h"
#include "vox.math/bounding_plane3.h"
#include "vox.math/vector3.h"
#include "vox.render/rendering/render_element.h"
#include "vox.render/scene_forward.h"
#include "vox.render/shadow/enum/shadow_resolution.h"
#include "vox.render/shadow/shadow_slice_data.h"

namespace vox {
enum FrustumCorner {
    FarBottomLeft = 0,
    FarTopLeft = 1,
    FarTopRight = 2,
    FarBottomRight = 3,
    nearBottomLeft = 4,
    nearTopLeft = 5,
    nearTopRight = 6,
    nearBottomRight = 7,
    unknown = 8
};

class ShadowUtils {
public:
    // now max shadow sample tent is 5x5,atlas borderSize at least 3=ceil(2.5),and +1 pixel is for global border for no
    // cascade mode.
    static constexpr float atlasBorderSize = 4.f;

    static float shadowResolution(ShadowResolution value);

    static wgpu::TextureFormat shadowDepthFormat(ShadowResolution value);

    static bool cullingRenderBounds(const BoundingBox3F& bounds,
                                    uint32_t cullPlaneCount,
                                    const std::array<BoundingPlane3F, 10>& cullPlanes);

    static void shadowCullFrustum(Renderer* renderer,
                                  ShadowSliceData& shadowSliceData,
                                  std::vector<RenderElement>& opaqueQueue,
                                  std::vector<RenderElement>& alphaTestQueue,
                                  std::vector<RenderElement>& transparentQueue);

    static void getBoundSphereByFrustum(
            float near, float far, Camera* camera, const Vector3F& forward, ShadowSliceData& shadowSliceData);

    static void getDirectionLightShadowCullPlanes(const BoundingFrustum& cameraFrustum,
                                                  float splitDistance,
                                                  float cameraNear,
                                                  const Vector3F& direction,
                                                  ShadowSliceData& shadowSliceData);

    static void getDirectionalLightMatrices(const Vector3F& lightUp,
                                            const Vector3F& lightSide,
                                            const Vector3F& lightForward,
                                            float nearPlane,
                                            float shadowResolution,
                                            ShadowSliceData& shadowSliceData);

    static float getMaxTileResolutionInAtlas(float atlasWidth, float atlasHeight, uint32_t tileCount);

private:
    static std::array<Point3F, 8> _frustumCorners;
    static BoundingPlane3F _adjustNearPlane;
    static BoundingPlane3F _adjustFarPlane;
    static std::array<FrustumFace, 5> _backPlaneFaces;
    static Point3F _edgePlanePoint2;

    static constexpr std::array<std::array<FrustumFace, 4>, 6> _frustumPlaneNeighbors = {
            // near
            std::array<FrustumFace, 4>{FrustumFace::Left, FrustumFace::Right, FrustumFace::Top, FrustumFace::Bottom},
            // far
            std::array<FrustumFace, 4>{FrustumFace::Left, FrustumFace::Right, FrustumFace::Top, FrustumFace::Bottom},
            // left
            std::array<FrustumFace, 4>{FrustumFace::Near, FrustumFace::Far, FrustumFace::Top, FrustumFace::Bottom},
            // right
            std::array<FrustumFace, 4>{FrustumFace::Near, FrustumFace::Far, FrustumFace::Top, FrustumFace::Bottom},
            // bottom
            std::array<FrustumFace, 4>{FrustumFace::Near, FrustumFace::Far, FrustumFace::Left, FrustumFace::Right},
            // top
            std::array<FrustumFace, 4>{FrustumFace::Near, FrustumFace::Far, FrustumFace::Left, FrustumFace::Right}};

    static constexpr std::array<std::array<std::array<FrustumCorner, 2>, 6>, 6> _frustumTwoPlaneCorners = {
            // near
            {// near
             {std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomLeft, FrustumCorner::nearTopLeft},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopRight, FrustumCorner::nearBottomRight},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomRight, FrustumCorner::nearBottomLeft},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopLeft, FrustumCorner::nearTopRight}},

             // far
             {// near
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopLeft, FrustumCorner::FarBottomLeft},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomRight, FrustumCorner::FarTopRight},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomLeft, FrustumCorner::FarBottomRight},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopRight, FrustumCorner::FarTopLeft}},

             // left
             {// near
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopLeft, FrustumCorner::nearBottomLeft},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomLeft, FrustumCorner::FarTopLeft},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomLeft, FrustumCorner::FarBottomLeft},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopLeft, FrustumCorner::nearTopLeft}},

             // right
             {// near
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomRight, FrustumCorner::nearTopRight},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopRight, FrustumCorner::FarBottomRight},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomRight, FrustumCorner::nearBottomRight},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopRight, FrustumCorner::FarTopRight}},

             // bottom
             {// near
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomLeft, FrustumCorner::nearBottomRight},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomRight, FrustumCorner::FarBottomLeft},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::FarBottomLeft, FrustumCorner::nearBottomLeft},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::nearBottomRight, FrustumCorner::FarBottomRight},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown}},

             // top
             {// near
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopRight, FrustumCorner::nearTopLeft},
              // far
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopLeft, FrustumCorner::FarTopRight},
              // left
              std::array<FrustumCorner, 2>{FrustumCorner::nearTopLeft, FrustumCorner::FarTopLeft},
              // right
              std::array<FrustumCorner, 2>{FrustumCorner::FarTopRight, FrustumCorner::nearTopRight},
              // bottom
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown},
              // top
              std::array<FrustumCorner, 2>{FrustumCorner::unknown, FrustumCorner::unknown}}}};
};

}  // namespace vox