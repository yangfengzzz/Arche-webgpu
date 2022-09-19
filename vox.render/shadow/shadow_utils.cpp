//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_utils.h"

#include <vector>

#include "vox.geometry/matrix_utils.h"
#include "vox.math/collision_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/renderer.h"

namespace vox {
std::array<Point3F, 8> ShadowUtils::_frustumCorners{};
BoundingPlane3F ShadowUtils::_adjustNearPlane{};
BoundingPlane3F ShadowUtils::_adjustFarPlane{};
std::array<FrustumFace, 5> ShadowUtils::_backPlaneFaces{};
Point3F ShadowUtils::_edgePlanePoint2{};

float ShadowUtils::shadowResolution(ShadowResolution value) {
    switch (value) {
        case ShadowResolution::Low:
            return 512;
        case ShadowResolution::Medium:
            return 1024;
        case ShadowResolution::High:
            return 2048;
        case ShadowResolution::VeryHigh:
            return 4096;
    }
}

wgpu::TextureFormat ShadowUtils::shadowDepthFormat(ShadowResolution value) {
    switch (value) {
        case ShadowResolution::Low:
        case ShadowResolution::Medium:
        case ShadowResolution::High:
            return wgpu::TextureFormat::Depth16Unorm;
        case ShadowResolution::VeryHigh:
            return wgpu::TextureFormat::Depth32Float;
    }
}

bool ShadowUtils::cullingRenderBounds(const BoundingBox3F& bounds,
                                      uint32_t cullPlaneCount,
                                      const std::array<BoundingPlane3F, 10>& cullPlanes) {
    const auto& min = bounds.lower_corner;
    const auto& max = bounds.upper_corner;

    bool pass = true;
    for (uint32_t i = 0; i < cullPlaneCount; i++) {
        const auto& plane = cullPlanes[i];
        const auto& normal = plane.normal;
        if (normal.x * (normal.x > 0.0 ? min.x : max.x) + normal.y * (normal.y > 0.0 ? min.y : max.y) +
                    normal.z * (normal.z > 0.0 ? min.z : max.z) >
            -plane.distance) {
            pass = false;
            break;
        }
    }
    return pass;
}

void ShadowUtils::shadowCullFrustum(Renderer* renderer,
                                    ShadowSliceData& shadowSliceData,
                                    std::vector<RenderElement>& opaqueQueue,
                                    std::vector<RenderElement>& alphaTestQueue,
                                    std::vector<RenderElement>& transparentQueue) {
    if (renderer->castShadow && ShadowUtils::cullingRenderBounds(renderer->bounds(), shadowSliceData.cullPlaneCount,
                                                                 shadowSliceData.cullPlanes)) {
        renderer->setDistanceForSort(shadowSliceData.position.distanceTo(renderer->bounds().midPoint()));
        renderer->render(opaqueQueue, alphaTestQueue, transparentQueue);
    }
}

void ShadowUtils::getBoundSphereByFrustum(
        float near, float far, Camera* camera, const Vector3F& forward, ShadowSliceData& shadowSliceData) {
    float aspectRatio = camera->aspectRatio();
    float fieldOfView = camera->fieldOfView();

    // https://lxjk.github.io/2017/04/15/Calculate-Minimal-Bounding-Sphere-of-Frustum.html
    float centerZ;
    float radius;
    float k = std::sqrt(1.f + aspectRatio * aspectRatio) * std::tan(degreesToRadians(fieldOfView) / 2.f);
    float k2 = k * k;
    float farSNear = far - near;
    float farANear = far + near;
    if (k2 > farSNear / farANear) {
        centerZ = far;
        radius = far * k;
    } else {
        centerZ = 0.5f * farANear * (1 + k2);
        radius = 0.5f *
                 std::sqrt(farSNear * farSNear + 2.f * (far * far + near * near) * k2 + farANear * farANear * k2 * k2);
    }
    const auto& worldPosition = camera->entity()->transform->worldPosition();

    shadowSliceData.splitBoundSphere.radius = radius;
    shadowSliceData.splitBoundSphere.center = worldPosition + forward * centerZ;
    shadowSliceData.sphereCenterZ = centerZ;
}

void ShadowUtils::getDirectionLightShadowCullPlanes(const BoundingFrustum& cameraFrustum,
                                                    float splitDistance,
                                                    float cameraNear,
                                                    const Vector3F& direction,
                                                    ShadowSliceData& shadowSliceData) {
    // http://lspiroengine.com/?p=187
    // cameraFrustumPlanes is share
    const auto& near = cameraFrustum.getPlane(FrustumFace::Near);
    const auto& far = cameraFrustum.getPlane(FrustumFace::Far);
    const auto& left = cameraFrustum.getPlane(FrustumFace::Left);
    const auto& right = cameraFrustum.getPlane(FrustumFace::Right);
    const auto& bottom = cameraFrustum.getPlane(FrustumFace::Bottom);
    const auto& top = cameraFrustum.getPlane(FrustumFace::Top);

    // adjustment the near/far plane
    float splitNearDistance = splitDistance - cameraNear;
    auto& splitNear = ShadowUtils::_adjustNearPlane;
    auto& splitFar = ShadowUtils::_adjustFarPlane;
    splitNear.normal = near.normal;
    splitFar.normal = far.normal;
    splitNear.distance = near.distance + splitNearDistance;
    // do a clamp is the sphere is out of range the far plane
    splitFar.distance = std::max(
            -near.distance - shadowSliceData.sphereCenterZ - shadowSliceData.splitBoundSphere.radius, far.distance);

    _frustumCorners[FrustumCorner::nearBottomRight] = intersectionPointThreePlanes(splitNear, bottom, right);
    _frustumCorners[FrustumCorner::nearTopRight] = intersectionPointThreePlanes(splitNear, top, right);
    _frustumCorners[FrustumCorner::nearTopLeft] = intersectionPointThreePlanes(splitNear, top, left);
    _frustumCorners[FrustumCorner::nearBottomLeft] = intersectionPointThreePlanes(splitNear, bottom, left);
    _frustumCorners[FrustumCorner::FarBottomRight] = intersectionPointThreePlanes(splitFar, bottom, right);
    _frustumCorners[FrustumCorner::FarTopRight] = intersectionPointThreePlanes(splitFar, top, right);
    _frustumCorners[FrustumCorner::FarTopLeft] = intersectionPointThreePlanes(splitFar, top, left);
    _frustumCorners[FrustumCorner::FarBottomLeft] = intersectionPointThreePlanes(splitFar, bottom, left);

    uint32_t backIndex = 0;
    for (uint32_t i = 0; i < 6; i++) {
        // maybe 3、4、5(light eye is at far, forward is near, or orthographic camera is any axis)
        BoundingPlane3F plane;
        switch (i) {
            case FrustumFace::Near:
                plane = splitNear;
                break;
            case FrustumFace::Far:
                plane = splitFar;
                break;
            default:
                plane = cameraFrustum.getPlane(FrustumFace(i));
                break;
        }
        if (plane.normal.dot(direction) < 0.0) {
            shadowSliceData.cullPlanes[backIndex] = plane;
            _backPlaneFaces[backIndex] = FrustumFace(i);
            backIndex++;
        }
    }

    uint32_t edgeIndex = backIndex;
    for (uint32_t i = 0; i < backIndex; i++) {
        const auto& backFace = _backPlaneFaces[i];
        const auto& neighborFaces = _frustumPlaneNeighbors[backFace];
        for (uint32_t j = 0; j < 4; j++) {
            const auto& neighborFace = neighborFaces[j];
            bool notBackFace = true;
            for (uint32_t k = 0; k < backIndex; k++)
                if (neighborFace == _backPlaneFaces[k]) {
                    notBackFace = false;
                    break;
                }
            if (notBackFace) {
                const auto& corners = _frustumTwoPlaneCorners[backFace][neighborFace];
                const auto& point0 = _frustumCorners[corners[0]];
                const auto& point1 = _frustumCorners[corners[1]];
                _edgePlanePoint2 = point0 + direction;
                shadowSliceData.cullPlanes[edgeIndex++] = BoundingPlane3F(point0, point1, _edgePlanePoint2);
            }
        }
    }
    shadowSliceData.cullPlaneCount = edgeIndex;
}

void ShadowUtils::getDirectionalLightMatrices(const Vector3F& lightUp,
                                              const Vector3F& lightSide,
                                              const Vector3F& lightForward,
                                              float nearPlane,
                                              float shadowResolution,
                                              ShadowSliceData& shadowSliceData) {
    auto& boundSphere = shadowSliceData.splitBoundSphere;
    shadowSliceData.resolution = shadowResolution;

    // To solve shadow swimming problem.
    auto& center = boundSphere.center;
    float radius = boundSphere.radius;
    float halfShadowResolution = shadowResolution / 2;
    // Add border to project edge pixel PCF.
    // Improve:the clip planes not consider the border,but I think is OK,because the object can clip is not continuous.
    const float borderRadius = (radius * halfShadowResolution) / (halfShadowResolution - ShadowUtils::atlasBorderSize);
    const float borderDiam = borderRadius * 2.f;
    const float sizeUnit = shadowResolution / borderDiam;
    const float radiusUnit = borderDiam / shadowResolution;
    const float upLen = std::ceil(center.dot(lightUp) * sizeUnit) * radiusUnit;
    const float sideLen = std::ceil(center.dot(lightSide) * sizeUnit) * radiusUnit;
    const float forwardLen = center.dot(lightForward);
    center.x = lightUp.x * upLen + lightSide.x * sideLen + lightForward.x * forwardLen;
    center.y = lightUp.y * upLen + lightSide.y * sideLen + lightForward.y * forwardLen;
    center.z = lightUp.z * upLen + lightSide.z * sideLen + lightForward.z * forwardLen;

    // Direction light use shadow pancaking tech,do special dispose with nearPlane.
    shadowSliceData.position = center - lightForward * (radius + nearPlane);
    shadowSliceData.viewMatrix = makeLookAtMatrix(shadowSliceData.position, center, lightUp);
    shadowSliceData.projectionMatrix = makeOrtho(-borderRadius, borderRadius, -borderRadius, borderRadius, 0.f, radius * 2.f + nearPlane);
    shadowSliceData.viewProjectMatrix = shadowSliceData.projectionMatrix * shadowSliceData.viewMatrix;
}

float ShadowUtils::getMaxTileResolutionInAtlas(float atlasWidth, float atlasHeight, uint32_t tileCount) {
    float resolution = std::min(atlasWidth, atlasHeight);
    float currentTileCount = std::floor(atlasWidth / resolution) * std::floor(atlasHeight / resolution);

    while (uint32_t(currentTileCount) < tileCount) {
        resolution = std::floor(uint32_t(resolution) >> 1);
        currentTileCount = std::floor(atlasWidth / resolution) * std::floor(atlasHeight / resolution);
    }
    return resolution;
}

}  // namespace vox