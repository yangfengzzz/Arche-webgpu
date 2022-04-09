//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "region.h"
#include <glog/logging.h>

namespace vox {
namespace geo {
const Region Region::InvalidRegion = Region(0, -1);

void Region::update() {
    _width.x = _maxs.x - _mins.x;
    _width.y = _maxs.y - _mins.y;
    _width.z = _maxs.z - _mins.z;
    _center.x = _mins.x + _width.x / 2;
    _center.y = _mins.y + _width.y / 2;
    _center.z = _mins.z + _width.z / 2;
    _voxels = _width + (ssize_t)1;
    _stride = _voxels.x * _voxels.y;
    _pivot = Vector3F(std::floor((float)_voxels.x / 2.0f), std::floor((float)_voxels.y / 2.0f), std::floor((float)_voxels.z / 2.0f));
}

std::string Region::toString(bool center) const {
    std::string regionStr("region[");
    Point3I s;
    if (center) {
        s = getCenter();
        regionStr += "center(";
        regionStr += std::to_string(s.x) + ", " + std::to_string(s.y) + ", "+ std::to_string(s.z);
        regionStr += "), ";
    }
    s = getLowerCorner();
    regionStr += "mins(";
    regionStr += std::to_string(s.x) + ", " + std::to_string(s.y) + ", "+ std::to_string(s.z);
    regionStr += "), ";
    s = getUpperCorner();
    regionStr += "maxs(";
    regionStr += std::to_string(s.x) + ", " + std::to_string(s.y) + ", "+ std::to_string(s.z);
    regionStr += ")]";
    return regionStr;
}

Point3I Region::moveInto(ssize_t x, ssize_t y, ssize_t z) const {
    const Point3I& size = getDimensionsInVoxels();
    const Point3I& mins = getLowerCorner();
    const Point3I& maxs = getUpperCorner();
    const ssize_t ox = (x < 0 ? maxs.x : mins.x) + (x % size.x);
    const ssize_t oy = (y < 0 ? maxs.y : mins.y) + (y % size.y);
    const ssize_t oz = (z < 0 ? maxs.z : mins.z) + (z % size.z);
    if (containsPoint(ox, oy, oz)) {
        throw std::invalid_argument("shifted is outside the valid region");
    }
    return Point3I(ox, oy, oz);
}

void logRegion(const char *ctx, const Region& region) {
    const Point3I& mins = region.getLowerCorner();
    const Point3I& maxs = region.getUpperCorner();
    LOG(INFO) << ctx << ": region[mins(" << mins.x << "," << mins.y << "," << mins.z << ")"
    << ", maxs(" << maxs.x << "" << maxs.y << "" << maxs.z << ")]";
}

ssize_t Region::voxels() const {
    return getWidthInVoxels() * getHeightInVoxels() * getDepthInVoxels();
}

Point3I Region::getRandomPosition(Random& random) const {
    const ssize_t x = random.random(_mins.x, _maxs.x);
    const ssize_t y = random.random(_mins.y, _maxs.y);
    const ssize_t z = random.random(_mins.z, _maxs.z);
    return Point3I(x, y, z);
}

/**
 * @param x The 'x' component of the position to accumulate.
 * @param y The 'y' component of the position to accumulate.
 * @param z The 'z' component of the position to accumulate.
 */
void Region::accumulate(ssize_t x, ssize_t y, ssize_t z) {
    _mins.x = std::min(_mins.x, x);
    _mins.y = std::min(_mins.y, y);
    _mins.z = std::min(_mins.z, z);
    _maxs.x = std::max(_maxs.x, x);
    _maxs.y = std::max(_maxs.y, y);
    _maxs.z = std::max(_maxs.z, z);
    update();
}

/**
 * Note that this is not the same as computing the union of two Regions (as the result of
 * such a union may not be a shape which can be exactly represented by a Region). Instead,
 * the result is simply big enough to contain both this Region and the one passed as a parameter.
 * @param reg The Region to accumulate. This must be valid as defined by the isValid() function.
 * @sa isValid()
 */
void Region::accumulate(const Region& reg) {
    if (!reg.isValid()) {
        // The result of accumulating an invalid region is not defined.
        throw std::invalid_argument("You cannot accumulate an invalid region.");
    }

    _mins = min(_mins, reg.getLowerCorner());
    _maxs = max(_maxs, reg.getUpperCorner());
    update();
}

/**
 * After calling this functions, the extents of this Region are given by the intersection
 * of this Region and the one it was cropped to.
 * @param other The Region to crop to.
 */
void Region::cropTo(const Region& other) {
    _mins = max(_mins, other._mins);
    _maxs = min(_maxs, other._maxs);
    update();
}

/**
 * The same amount of growth is applied in all directions. Negative growth
 * is possible but you should prefer the shrink() function for clarity.
 * @param amount The amount to grow by.
 */
void Region::grow(ssize_t amount) {
    _mins -= amount;
    _maxs += amount;
    update();
}

/**
 * The amount can be specified separately for each direction. Negative growth
 * is possible but you should prefer the shrink() function for clarity.
 * @param x The amount to grow by in 'x'.
 * @param y The amount to grow by in 'y'.
 * @param z The amount to grow by in 'z'.
 */
void Region::grow(ssize_t x, ssize_t y, ssize_t z) {
    _mins.x -= x;
    _mins.y -= y;
    _mins.z -= z;

    _maxs.x += x;
    _maxs.y += y;
    _maxs.z += z;
    update();
}

/**
 * The amount can be specified separately for each direction. Negative growth
 * is possible but you should prefer the shrink() function for clarity.
 * @param amount The amount to grow by (one component for each direction).
 */
void Region::grow(const Point3I& amount) {
    _mins.x -= amount.x;
    _mins.y -= amount.y;
    _mins.z -= amount.z;
    
    _maxs.x += amount.x;
    _maxs.y += amount.y;
    _maxs.z += amount.z;
    update();
}

/**
 * @return The position of the lower corner.
 */
const Point3I& Region::getCenter() const {
    return _center;
}

const Vector3F& Region::getPivot() const {
    return _pivot;
}

/**
 * @return The position of the lower corner.
 */
const Point3I& Region::getLowerCorner() const {
    return _mins;
}

/**
 * @return The position of the upper corner.
 */
const Point3I& Region::getUpperCorner() const {
    return _maxs;
}

Vector3F Region::getLowerCornerf() const {
    return Vector3F(_mins.x, _mins.y, _mins.z);
}

Vector3F Region::getUpperCornerf() const {
    return Vector3F(_maxs.x, _maxs.y, _maxs.z);
}

/**
 * @return The dimensions of the region measured in voxels.
 * @sa getDimensionsInCells()
 */
const Point3I& Region::getDimensionsInVoxels() const {
    return _voxels;
}

/**
 * @return The dimensions of the region measured in cells.
 * @sa getDimensionsInVoxels()
 */
const Point3I& Region::getDimensionsInCells() const {
    return _width;
}

/**
 * @param mins The new position of the lower corner.
 */
void Region::setLowerCorner(const Point3I& mins) {
    _mins = mins;
    update();
}

/**
 * @param maxs The new position of the upper corner.
 */
void Region::setUpperCorner(const Point3I& maxs) {
    _maxs = maxs;
    update();
}

/**
 * @param pos The position to accumulate.
 */
void Region::accumulate(const Point3I& pos) {
    accumulate(pos.x, pos.y, pos.z);
}

/**
 * Constructs a Region and sets the lower and upper corners to the specified values.
 * @param mins The desired lower corner of the Region.
 * @param maxs The desired upper corner of the Region.
 */
Region::Region(const Point3I& mins, const Point3I& maxs) :
        Region(mins.x, mins.y, mins.z, maxs.x, maxs.y, maxs.z) {
}

Region& Region::operator+=(const Point3I& amount) {
    shift(amount);
    return *this;
}

Region operator+(const Region& region, const Point3I& amount) {
    Region copy(region);
    copy.shift(amount);
    return copy;
}

/**
 * The boundary value can be used to ensure a position is only considered to be inside
 * the Region if it is that far in in all directions. Also, the test is inclusive such
 * that positions lying exactly on the edge of the Region are considered to be inside it.
 * @param pos The position to test.
 * @param boundary The desired boundary value.
 */
bool Region::containsPoint(const Vector3F& pos, float boundary) const {
    return containsPoint(pos.x, pos.y, pos.z, boundary);
}

/**
 * The boundary value can be used to ensure a position is only considered to be inside
 * the Region if it is that far in in all directions. Also, the test is inclusive such
 * that positions lying exactly on the edge of the Region are considered to be inside it.
 * @param pos The position to test.
 * @param boundary The desired boundary value.
 */
bool Region::containsPoint(const Point3I& pos, uint8_t boundary) const {
    return containsPoint(pos.x, pos.y, pos.z, boundary);
}

/**
 * @param amount The amount to move the Region by.
 */
void Region::shift(const Point3I& amount) {
    shiftLowerCorner(amount);
    shiftUpperCorner(amount);
}

/**
 * @param amount The amount to move the lower corner by.
 */
void Region::shiftLowerCorner(const Point3I& amount) {
    shiftLowerCorner(amount.x, amount.y, amount.z);
}

/**
 * @param amount The amount to move the upper corner by.
 */
void Region::shiftUpperCorner(const Point3I& amount) {
    shiftUpperCorner(amount.x, amount.y, amount.z);
}

/**
 * The amount can be specified seperatly for each direction. Negative shrinkage
 * is possible but you should prefer the grow() function for clarity.
 * @param amount The amount to shrink by (one component for each direction).
 */
void Region::shrink(const Point3I& amount) {
    shrink(amount.x, amount.y, amount.z);
}



}
}
