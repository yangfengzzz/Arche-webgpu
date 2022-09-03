// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/point_hash_grid_searcher3.h"

#include <algorithm>
#include <vector>

#include "vox.base/private_helpers.h"
#include "vox.geometry/array1.h"

using namespace vox;

PointHashGridSearcher3::PointHashGridSearcher3(const Size3& resolution, double gridSpacing)
    : PointHashGridSearcher3(resolution.x, resolution.y, resolution.z, gridSpacing) {}

PointHashGridSearcher3::PointHashGridSearcher3(size_t resolutionX,
                                               size_t resolutionY,
                                               size_t resolutionZ,
                                               double gridSpacing)
    : _gridSpacing(gridSpacing) {
    _resolution.x = std::max(static_cast<ssize_t>(resolutionX), kOneSSize);
    _resolution.y = std::max(static_cast<ssize_t>(resolutionY), kOneSSize);
    _resolution.z = std::max(static_cast<ssize_t>(resolutionZ), kOneSSize);
}

PointHashGridSearcher3::PointHashGridSearcher3(const PointHashGridSearcher3& other) { set(other); }

void PointHashGridSearcher3::build(const ConstArrayAccessor1<Point3D>& points) {
    _buckets.clear();
    _points.clear();

    // Allocate memory chuncks
    _buckets.resize(_resolution.x * _resolution.y * _resolution.z);
    _points.resize(points.size());

    if (points.size() == 0) {
        return;
    }

    // Put points into buckets
    for (size_t i = 0; i < points.size(); ++i) {
        _points[i] = points[i];
        size_t key = getHashKeyFromPosition(points[i]);
        _buckets[key].push_back(i);
    }
}

void PointHashGridSearcher3::forEachNearbyPoint(const Point3D& origin,
                                                double radius,
                                                const std::function<void(size_t, const Point3D&)>& callback) const {
    if (_buckets.empty()) {
        return;
    }

    size_t nearbyKeys[8];
    getNearbyKeys(origin, nearbyKeys);

    const double queryRadiusSquared = radius * radius;

    for (unsigned long nearbyKey : nearbyKeys) {
        const auto& bucket = _buckets[nearbyKey];
        size_t numberOfPointsInBucket = bucket.size();

        for (size_t j = 0; j < numberOfPointsInBucket; ++j) {
            size_t pointIndex = bucket[j];
            double rSquared = (_points[pointIndex] - origin).lengthSquared();
            if (rSquared <= queryRadiusSquared) {
                callback(pointIndex, _points[pointIndex]);
            }
        }
    }
}

bool PointHashGridSearcher3::hasNearbyPoint(const Point3D& origin, double radius) const {
    if (_buckets.empty()) {
        return false;
    }

    size_t nearbyKeys[8];
    getNearbyKeys(origin, nearbyKeys);

    const double queryRadiusSquared = radius * radius;

    for (unsigned long nearbyKey : nearbyKeys) {
        const auto& bucket = _buckets[nearbyKey];
        size_t numberOfPointsInBucket = bucket.size();

        for (size_t j = 0; j < numberOfPointsInBucket; ++j) {
            size_t pointIndex = bucket[j];
            double rSquared = (_points[pointIndex] - origin).lengthSquared();
            if (rSquared <= queryRadiusSquared) {
                return true;
            }
        }
    }

    return false;
}

void PointHashGridSearcher3::add(const Point3D& point) {
    if (_buckets.empty()) {
        Array1<Point3D> arr = {point};
        build(arr.constAccessor());
    } else {
        size_t i = _points.size();
        _points.push_back(point);
        size_t key = getHashKeyFromPosition(point);
        _buckets[key].push_back(i);
    }
}

const std::vector<std::vector<size_t>>& PointHashGridSearcher3::buckets() const { return _buckets; }

Point3I PointHashGridSearcher3::getBucketIndex(const Point3D& position) const {
    Point3I bucketIndex;
    bucketIndex.x = static_cast<ssize_t>(std::floor(position.x / _gridSpacing));
    bucketIndex.y = static_cast<ssize_t>(std::floor(position.y / _gridSpacing));
    bucketIndex.z = static_cast<ssize_t>(std::floor(position.z / _gridSpacing));
    return bucketIndex;
}

size_t PointHashGridSearcher3::getHashKeyFromPosition(const Point3D& position) const {
    Point3I bucketIndex = getBucketIndex(position);
    return getHashKeyFromBucketIndex(bucketIndex);
}

size_t PointHashGridSearcher3::getHashKeyFromBucketIndex(const Point3I& bucketIndex) const {
    Point3I wrappedIndex = bucketIndex;
    wrappedIndex.x = bucketIndex.x % _resolution.x;
    wrappedIndex.y = bucketIndex.y % _resolution.y;
    wrappedIndex.z = bucketIndex.z % _resolution.z;
    if (wrappedIndex.x < 0) {
        wrappedIndex.x += _resolution.x;
    }
    if (wrappedIndex.y < 0) {
        wrappedIndex.y += _resolution.y;
    }
    if (wrappedIndex.z < 0) {
        wrappedIndex.z += _resolution.z;
    }
    return static_cast<size_t>((wrappedIndex.z * _resolution.y + wrappedIndex.y) * _resolution.x + wrappedIndex.x);
}

void PointHashGridSearcher3::getNearbyKeys(const Point3D& position, size_t* nearbyKeys) const {
    Point3I originIndex = getBucketIndex(position), nearbyBucketIndices[8];

    for (auto& nearbyBucketIndice : nearbyBucketIndices) {
        nearbyBucketIndice = originIndex;
    }

    if ((originIndex.x + 0.5f) * _gridSpacing <= position.x) {
        nearbyBucketIndices[4].x += 1;
        nearbyBucketIndices[5].x += 1;
        nearbyBucketIndices[6].x += 1;
        nearbyBucketIndices[7].x += 1;
    } else {
        nearbyBucketIndices[4].x -= 1;
        nearbyBucketIndices[5].x -= 1;
        nearbyBucketIndices[6].x -= 1;
        nearbyBucketIndices[7].x -= 1;
    }

    if ((originIndex.y + 0.5f) * _gridSpacing <= position.y) {
        nearbyBucketIndices[2].y += 1;
        nearbyBucketIndices[3].y += 1;
        nearbyBucketIndices[6].y += 1;
        nearbyBucketIndices[7].y += 1;
    } else {
        nearbyBucketIndices[2].y -= 1;
        nearbyBucketIndices[3].y -= 1;
        nearbyBucketIndices[6].y -= 1;
        nearbyBucketIndices[7].y -= 1;
    }

    if ((originIndex.z + 0.5f) * _gridSpacing <= position.z) {
        nearbyBucketIndices[1].z += 1;
        nearbyBucketIndices[3].z += 1;
        nearbyBucketIndices[5].z += 1;
        nearbyBucketIndices[7].z += 1;
    } else {
        nearbyBucketIndices[1].z -= 1;
        nearbyBucketIndices[3].z -= 1;
        nearbyBucketIndices[5].z -= 1;
        nearbyBucketIndices[7].z -= 1;
    }

    for (int i = 0; i < 8; i++) {
        nearbyKeys[i] = getHashKeyFromBucketIndex(nearbyBucketIndices[i]);
    }
}

PointNeighborSearcher3Ptr PointHashGridSearcher3::clone() const {return CLONE_W_CUSTOM_DELETER(PointHashGridSearcher3)}

PointHashGridSearcher3& PointHashGridSearcher3::operator=(const PointHashGridSearcher3& other) {
    set(other);
    return *this;
}

void PointHashGridSearcher3::set(const PointHashGridSearcher3& other) {
    _gridSpacing = other._gridSpacing;
    _resolution = other._resolution;
    _points = other._points;
    _buckets = other._buckets;
}

PointHashGridSearcher3::Builder PointHashGridSearcher3::builder() { return Builder(); }

PointHashGridSearcher3::Builder& PointHashGridSearcher3::Builder::withResolution(const Size3& resolution) {
    _resolution = resolution;
    return *this;
}

PointHashGridSearcher3::Builder& PointHashGridSearcher3::Builder::withGridSpacing(double gridSpacing) {
    _gridSpacing = gridSpacing;
    return *this;
}

PointHashGridSearcher3 PointHashGridSearcher3::Builder::build() const {
    return PointHashGridSearcher3(_resolution, _gridSpacing);
}

PointHashGridSearcher3Ptr PointHashGridSearcher3::Builder::makeShared() const {
    return std::shared_ptr<PointHashGridSearcher3>(new PointHashGridSearcher3(_resolution, _gridSpacing),
                                                   [](PointHashGridSearcher3* obj) { delete obj; });
}

PointNeighborSearcher3Ptr PointHashGridSearcher3::Builder::buildPointNeighborSearcher() const { return makeShared(); }
