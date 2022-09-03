// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/point_parallel_hash_grid_searcher2.h"

#include <algorithm>
#include <vector>

#include "vox.base/constants.h"
#include "vox.base/logging.h"
#include "vox.base/parallel.h"
#include "vox.base/private_helpers.h"

using namespace vox;

PointParallelHashGridSearcher2::PointParallelHashGridSearcher2(const Size2& resolution, double gridSpacing)
    : PointParallelHashGridSearcher2(resolution.x, resolution.y, gridSpacing) {}

PointParallelHashGridSearcher2::PointParallelHashGridSearcher2(size_t resolutionX,
                                                               size_t resolutionY,
                                                               double gridSpacing)
    : _gridSpacing(gridSpacing) {
    _resolution.x = std::max(static_cast<ssize_t>(resolutionX), kOneSSize);
    _resolution.y = std::max(static_cast<ssize_t>(resolutionY), kOneSSize);

    _startIndexTable.resize(_resolution.x * _resolution.y, kMaxSize);
    _endIndexTable.resize(_resolution.x * _resolution.y, kMaxSize);
}

PointParallelHashGridSearcher2::PointParallelHashGridSearcher2(const PointParallelHashGridSearcher2& other) {
    set(other);
}

void PointParallelHashGridSearcher2::build(const ConstArrayAccessor1<Point2D>& points) {
    _points.clear();
    _keys.clear();
    _startIndexTable.clear();
    _endIndexTable.clear();
    _sortedIndices.clear();

    // Allocate memory chuncks
    size_t numberOfPoints = points.size();
    std::vector<size_t> tempKeys(numberOfPoints);
    _startIndexTable.resize(_resolution.x * _resolution.y);
    _endIndexTable.resize(_resolution.x * _resolution.y);
    parallelFill(_startIndexTable.begin(), _startIndexTable.end(), kMaxSize);
    parallelFill(_endIndexTable.begin(), _endIndexTable.end(), kMaxSize);
    _keys.resize(numberOfPoints);
    _sortedIndices.resize(numberOfPoints);
    _points.resize(numberOfPoints);

    if (numberOfPoints == 0) {
        return;
    }

    // Initialize indices array and generate hash key for each point
    parallelFor(kZeroSize, numberOfPoints, [&](size_t i) {
        _sortedIndices[i] = i;
        _points[i] = points[i];
        tempKeys[i] = getHashKeyFromPosition(points[i]);
    });

    // Sort indices based on hash key
    parallelSort(_sortedIndices.begin(), _sortedIndices.end(),
                 [&tempKeys](size_t indexA, size_t indexB) { return tempKeys[indexA] < tempKeys[indexB]; });

    // Re-order point and key arrays
    parallelFor(kZeroSize, numberOfPoints, [&](size_t i) {
        _points[i] = points[_sortedIndices[i]];
        _keys[i] = tempKeys[_sortedIndices[i]];
    });

    // Now _points and _keys are sorted by points' hash key values.
    // Let's fill in start/end index table with _keys.

    // Assume that _keys array looks like:
    // [5|8|8|10|10|10]
    // Then _startIndexTable and _endIndexTable should be like:
    // [.....|0|...|1|..|3|..]
    // [.....|1|...|3|..|6|..]
    //       ^5    ^8   ^10
    // So that _endIndexTable[i] - _startIndexTable[i] is the number points
    // in i-th table bucket.

    _startIndexTable[_keys[0]] = 0;
    _endIndexTable[_keys[numberOfPoints - 1]] = numberOfPoints;

    parallelFor((size_t)1, numberOfPoints, [&](size_t i) {
        if (_keys[i] > _keys[i - 1]) {
            _startIndexTable[_keys[i]] = i;
            _endIndexTable[_keys[i - 1]] = i;
        }
    });

    size_t sumNumberOfPointsPerBucket = 0;
    size_t maxNumberOfPointsPerBucket = 0;
    size_t numberOfNonEmptyBucket = 0;
    for (size_t i = 0; i < _startIndexTable.size(); ++i) {
        if (_startIndexTable[i] != kMaxSize) {
            size_t numberOfPointsInBucket = _endIndexTable[i] - _startIndexTable[i];
            sumNumberOfPointsPerBucket += numberOfPointsInBucket;
            maxNumberOfPointsPerBucket = std::max(maxNumberOfPointsPerBucket, numberOfPointsInBucket);
            ++numberOfNonEmptyBucket;
        }
    }

    LOGI("Average number of points per non-empty bucket: {}",
         static_cast<float>(sumNumberOfPointsPerBucket) / static_cast<float>(numberOfNonEmptyBucket))
    LOGI("Max number of points per bucket: {}", maxNumberOfPointsPerBucket)
}

void PointParallelHashGridSearcher2::forEachNearbyPoint(const Point2D& origin,
                                                        double radius,
                                                        const ForEachNearbyPointFunc& callback) const {
    size_t nearbyKeys[4];
    getNearbyKeys(origin, nearbyKeys);

    const double queryRadiusSquared = radius * radius;

    for (unsigned long nearbyKey : nearbyKeys) {
        size_t start = _startIndexTable[nearbyKey];
        size_t end = _endIndexTable[nearbyKey];

        // Empty bucket -- continue to next bucket
        if (start == kMaxSize) {
            continue;
        }

        for (size_t j = start; j < end; ++j) {
            Vector2D direction = _points[j] - origin;
            double distanceSquared = direction.lengthSquared();
            if (distanceSquared <= queryRadiusSquared) {
                callback(_sortedIndices[j], _points[j]);
            }
        }
    }
}

bool PointParallelHashGridSearcher2::hasNearbyPoint(const Point2D& origin, double radius) const {
    size_t nearbyKeys[4];
    getNearbyKeys(origin, nearbyKeys);

    const double queryRadiusSquared = radius * radius;

    for (unsigned long nearbyKey : nearbyKeys) {
        size_t start = _startIndexTable[nearbyKey];
        size_t end = _endIndexTable[nearbyKey];

        // Empty bucket -- continue to next bucket
        if (start == kMaxSize) {
            continue;
        }

        for (size_t j = start; j < end; ++j) {
            Vector2D direction = _points[j] - origin;
            double distanceSquared = direction.lengthSquared();
            if (distanceSquared <= queryRadiusSquared) {
                return true;
            }
        }
    }

    return false;
}

const std::vector<size_t>& PointParallelHashGridSearcher2::keys() const { return _keys; }

const std::vector<size_t>& PointParallelHashGridSearcher2::startIndexTable() const { return _startIndexTable; }

const std::vector<size_t>& PointParallelHashGridSearcher2::endIndexTable() const { return _endIndexTable; }

const std::vector<size_t>& PointParallelHashGridSearcher2::sortedIndices() const { return _sortedIndices; }

Point2I PointParallelHashGridSearcher2::getBucketIndex(const Point2D& position) const {
    Point2I bucketIndex;
    bucketIndex.x = static_cast<ssize_t>(std::floor(position.x / _gridSpacing));
    bucketIndex.y = static_cast<ssize_t>(std::floor(position.y / _gridSpacing));
    return bucketIndex;
}

size_t PointParallelHashGridSearcher2::getHashKeyFromPosition(const Point2D& position) const {
    Point2I bucketIndex = getBucketIndex(position);

    return getHashKeyFromBucketIndex(bucketIndex);
}

size_t PointParallelHashGridSearcher2::getHashKeyFromBucketIndex(const Point2I& bucketIndex) const {
    Point2I wrappedIndex = bucketIndex;
    wrappedIndex.x = bucketIndex.x % _resolution.x;
    wrappedIndex.y = bucketIndex.y % _resolution.y;
    if (wrappedIndex.x < 0) {
        wrappedIndex.x += _resolution.x;
    }
    if (wrappedIndex.y < 0) {
        wrappedIndex.y += _resolution.y;
    }
    return static_cast<size_t>(wrappedIndex.y * _resolution.x + wrappedIndex.x);
}

void PointParallelHashGridSearcher2::getNearbyKeys(const Point2D& position, size_t* nearbyKeys) const {
    Point2I originIndex = getBucketIndex(position), nearbyBucketIndices[4];

    for (auto& nearbyBucketIndice : nearbyBucketIndices) {
        nearbyBucketIndice = originIndex;
    }

    if ((originIndex.x + 0.5f) * _gridSpacing <= position.x) {
        nearbyBucketIndices[2].x += 1;
        nearbyBucketIndices[3].x += 1;
    } else {
        nearbyBucketIndices[2].x -= 1;
        nearbyBucketIndices[3].x -= 1;
    }

    if ((originIndex.y + 0.5f) * _gridSpacing <= position.y) {
        nearbyBucketIndices[1].y += 1;
        nearbyBucketIndices[3].y += 1;
    } else {
        nearbyBucketIndices[1].y -= 1;
        nearbyBucketIndices[3].y -= 1;
    }

    for (int i = 0; i < 4; i++) {
        nearbyKeys[i] = getHashKeyFromBucketIndex(nearbyBucketIndices[i]);
    }
}

PointNeighborSearcher2Ptr PointParallelHashGridSearcher2::clone() const {
        return CLONE_W_CUSTOM_DELETER(PointParallelHashGridSearcher2)}

PointParallelHashGridSearcher2& PointParallelHashGridSearcher2::operator=(const PointParallelHashGridSearcher2& other) {
    set(other);
    return *this;
}

void PointParallelHashGridSearcher2::set(const PointParallelHashGridSearcher2& other) {
    _gridSpacing = other._gridSpacing;
    _resolution = other._resolution;
    _points = other._points;
    _keys = other._keys;
    _startIndexTable = other._startIndexTable;
    _endIndexTable = other._endIndexTable;
    _sortedIndices = other._sortedIndices;
}

PointParallelHashGridSearcher2::Builder PointParallelHashGridSearcher2::builder() { return Builder(); }

PointParallelHashGridSearcher2::Builder& PointParallelHashGridSearcher2::Builder::withResolution(
        const Size2& resolution) {
    _resolution = resolution;
    return *this;
}

PointParallelHashGridSearcher2::Builder& PointParallelHashGridSearcher2::Builder::withGridSpacing(double gridSpacing) {
    _gridSpacing = gridSpacing;
    return *this;
}

PointParallelHashGridSearcher2 PointParallelHashGridSearcher2::Builder::build() const {
    return PointParallelHashGridSearcher2(_resolution, _gridSpacing);
}

PointParallelHashGridSearcher2Ptr PointParallelHashGridSearcher2::Builder::makeShared() const {
    return std::shared_ptr<PointParallelHashGridSearcher2>(
            new PointParallelHashGridSearcher2(_resolution, _gridSpacing),
            [](PointParallelHashGridSearcher2* obj) { delete obj; });
}

PointNeighborSearcher2Ptr PointParallelHashGridSearcher2::Builder::buildPointNeighborSearcher() const {
    return makeShared();
}
