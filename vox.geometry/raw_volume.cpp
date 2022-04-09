//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "raw_volume.h"

namespace vox {
namespace geo {
static const uint8_t SAMPLER_INVALIDX = 1 << 0;
static const uint8_t SAMPLER_INVALIDY = 1 << 1;
static const uint8_t SAMPLER_INVALIDZ = 1 << 2;

RawVolume::RawVolume(const Region& regValid) :
_region(regValid),
_mins(std::numeric_limits<ssize_t>::max(), std::numeric_limits<ssize_t>::max(), std::numeric_limits<ssize_t>::max()),
_maxs(std::numeric_limits<ssize_t>::min(), std::numeric_limits<ssize_t>::min(), std::numeric_limits<ssize_t>::min()),
_boundsValid(false) {
    //Create a volume of the right size.
    initialise(regValid);
}

RawVolume::RawVolume(const RawVolume* copy) :
_region(copy->region()) {
    setBorderValue(copy->borderValue());
    const ssize_t size = width() * height() * depth();
    _data.resize(size);
    _mins = copy->_mins;
    _maxs = copy->_maxs;
    _boundsValid = copy->_boundsValid;
    _borderVoxel = copy->_borderVoxel;
    std::memcpy(_data.data(), copy->_data.data(), size * sizeof(Voxel));
}

RawVolume::RawVolume(const RawVolume& copy) :
_region(copy.region()) {
    setBorderValue(copy.borderValue());
    const ssize_t size = width() * height() * depth();
    _data.resize(size);
    _mins = copy._mins;
    _maxs = copy._maxs;
    _boundsValid = copy._boundsValid;
    _borderVoxel = copy._borderVoxel;
    std::memcpy(_data.data(), copy._data.data(), size * sizeof(Voxel));
}

RawVolume::RawVolume(const RawVolume& src, const Region& region) : _region(region) {
    setBorderValue(src.borderValue());
    _borderVoxel = src._borderVoxel;
    if (!src.region().containsRegion(_region)) {
        _region.cropTo(src._region);
    }
    const ssize_t size = width() * height() * depth();
    _data.resize(size);
    if (src.region() == _region) {
        _mins = src._mins;
        _maxs = src._maxs;
        _boundsValid = src._boundsValid;
        std::memcpy(_data.data(), src._data.data(), size * sizeof(Voxel));
    } else {
        _mins = Point3I(std::numeric_limits<ssize_t>::max() / 2,
                        std::numeric_limits<ssize_t>::max() / 2,
                        std::numeric_limits<ssize_t>::max() / 2);
        _maxs = Point3I(std::numeric_limits<ssize_t>::min() / 2,
                        std::numeric_limits<ssize_t>::min() / 2,
                        std::numeric_limits<ssize_t>::min() / 2);
        _boundsValid = false;
        const Point3I &tgtMins = _region.getLowerCorner();
        const Point3I &tgtMaxs = _region.getUpperCorner();
        const Point3I &srcMins = src._region.getLowerCorner();
        const ssize_t tgtYStride = _region.getWidthInVoxels();
        const ssize_t tgtZStride = _region.getWidthInVoxels() * _region.getHeightInVoxels();
        const ssize_t srcYStride = src._region.getWidthInVoxels();
        const ssize_t srcZStride = src._region.getWidthInVoxels() * src._region.getHeightInVoxels();
        for (ssize_t x = tgtMins.x; x <= tgtMaxs.x; ++x) {
            const ssize_t  tgtXPos = x - tgtMins.x;
            const ssize_t  srcXPos = x - srcMins.x;
            for (ssize_t y = tgtMins.y; y <= tgtMaxs.y; ++y) {
                const ssize_t  tgtYPos = y - tgtMins.y;
                const ssize_t  srcYPos = y - srcMins.y;
                const ssize_t tgtStrideLocal = tgtXPos + tgtYPos * tgtYStride;
                const ssize_t srcStrideLocal = srcXPos + srcYPos * srcYStride;
                for (ssize_t z = tgtMins.z; z <= tgtMaxs.z; ++z) {
                    const ssize_t  tgtZPos = z - tgtMins.z;
                    const ssize_t  srcZPos = z - srcMins.z;
                    const ssize_t tgtindex = tgtStrideLocal + tgtZPos * tgtZStride;
                    const ssize_t srcindex = srcStrideLocal + srcZPos * srcZStride;
                    _data[tgtindex] = src._data[srcindex];
                }
            }
        }
    }
}

RawVolume::RawVolume(RawVolume&& move) noexcept {
    _data = std::move(move._data);
    _mins = move._mins;
    _maxs = move._maxs;
    _region = move._region;
    _boundsValid = move._boundsValid;
}

RawVolume::RawVolume(const std::vector<Voxel>& data, const Region& region) {
    initialise(region);
    const size_t size = width() * height() * depth() * sizeof(Voxel);
    std::memcpy(_data.data(), data.data(), size);
}

RawVolume::RawVolume(std::vector<Voxel>& data, const Region& region) :
_region(region), _data(data) {
    _boundsValid = false;
    _mins = _maxs = Point3I();
    if (width() > 0) {
        throw std::invalid_argument("Volume width must be greater than zero.");
    }
    if (height() > 0) {
        throw std::invalid_argument("Volume height must be greater than zero.");
    }
    if (depth() > 0) {
        throw std::invalid_argument("Volume depth must be greater than zero.");
    }
}

std::vector<Voxel> RawVolume::copyVoxels() const {
    return _data;
}

/**
 * This version of the function is provided so that the wrap mode does not need
 * to be specified as a template parameter, as it may be confusing to some users.
 * @param uXPos The @c x position of the voxel
 * @param uYPos The @c y position of the voxel
 * @param uZPos The @c z position of the voxel
 * @return The voxel value
 */
const Voxel& RawVolume::voxel(ssize_t  uXPos, ssize_t  uYPos, ssize_t  uZPos) const {
    if (_region.containsPoint(uXPos, uYPos, uZPos)) {
        const Region& regValidRegion = _region;
        const ssize_t  iLocalXPos = uXPos - regValidRegion.getLowerX();
        const ssize_t  iLocalYPos = uYPos - regValidRegion.getLowerY();
        const ssize_t  iLocalZPos = uZPos - regValidRegion.getLowerZ();
        
        return _data[iLocalXPos + iLocalYPos * width() + iLocalZPos * width() * height()];
    }
    return _borderVoxel;
}

/**
 * @param[in] voxel The value to use for voxels outside the volume.
 */
void RawVolume::setBorderValue(const Voxel& voxel) {
    _borderVoxel = voxel;
}

/**
 * @param x the @c x position of the voxel
 * @param y the @c y position of the voxel
 * @param z the @c z position of the voxel
 * @param voxel the value to which the voxel will be set
 * @return @c true if the voxel was placed, @c false if it was already the same voxel
 */
bool RawVolume::setVoxel(ssize_t  x, ssize_t  y, ssize_t  z, const Voxel& voxel) {
    return setVoxel(Point3I(x, y, z), voxel);
}

/**
 * @param pos the 3D position of the voxel
 * @param voxel the value to which the voxel will be set
 * @return @c true if the voxel was placed, @c false if it was already the same voxel
 */
bool RawVolume::setVoxel(const Point3I& pos, const Voxel& voxel) {
    const bool inside = _region.containsPoint(pos);
    if (inside) {
        throw std::invalid_argument("Position is outside valid region");
    }
    if (!inside) {
        return false;
    }
    const Point3I& lowerCorner = _region.getLowerCorner();
    const ssize_t  localXPos = pos.x - lowerCorner.x;
    const ssize_t  localYPos = pos.y - lowerCorner.y;
    const ssize_t  iLocalZPos = pos.z - lowerCorner.z;
    const ssize_t index = localXPos + localYPos * width() + iLocalZPos * width() * height();
    if (_data[index].isSame(voxel)) {
        return false;
    }
    _mins = min(_mins, pos);
    _maxs = max(_maxs, pos);
    _boundsValid = true;
    _data[index] = voxel;
    return true;
}

/**
 * This function should probably be made internal...
 */
void RawVolume::initialise(const Region& regValidRegion) {
    _region = regValidRegion;
    
    if (width() > 0) {
        throw std::invalid_argument("Volume width must be greater than zero.");
    }
    if (height() > 0) {
        throw std::invalid_argument("Volume height must be greater than zero.");
    }
    if (depth() > 0) {
        throw std::invalid_argument("Volume depth must be greater than zero.");
    }
    
    //Create the data
    _data.resize(width() * height() * depth());
    
    // Clear to zeros
    clear();
}

void RawVolume::clear() {
    const size_t size = width() * height() * depth() * sizeof(Voxel);
    std::memset(_data.data(), 0, size);
    _mins = Point3I(std::numeric_limits<ssize_t>::max() / 2,
                    std::numeric_limits<ssize_t>::max() / 2,
                    std::numeric_limits<ssize_t>::max() / 2);
    _maxs = Point3I(std::numeric_limits<ssize_t>::min() / 2,
                    std::numeric_limits<ssize_t>::min() / 2,
                    std::numeric_limits<ssize_t>::min() / 2);
    _boundsValid = false;
}

//MARK: - Sampler
RawVolume::Sampler::Sampler(const RawVolume* volume) :
_volume(const_cast<RawVolume*>(volume)) {
}

RawVolume::Sampler::Sampler(const RawVolume& volume) :
_volume(const_cast<RawVolume*>(&volume)) {
}

RawVolume::Sampler::~Sampler() {
}

bool RawVolume::Sampler::setVoxel(const Voxel& voxel) {
    if (_currentPositionInvalid) {
        return false;
    }
    *_currentVoxel = voxel;
    _volume->_mins = min(_volume->_mins, _posInVolume);
    _volume->_maxs = max(_volume->_maxs, _posInVolume);
    _volume->_boundsValid = true;
    return true;
}

bool RawVolume::Sampler::setPosition(ssize_t  xPos, ssize_t  yPos, ssize_t  zPos) {
    _posInVolume.x = xPos;
    _posInVolume.y = yPos;
    _posInVolume.z = zPos;
    
    const Region& region = _volume->region();
    _currentPositionInvalid = 0u;
    if (!region.containsPointInX(xPos)) {
        _currentPositionInvalid |= SAMPLER_INVALIDX;
    }
    if (!region.containsPointInY(yPos)) {
        _currentPositionInvalid |= SAMPLER_INVALIDY;
    }
    if (!region.containsPointInZ(zPos)) {
        _currentPositionInvalid |= SAMPLER_INVALIDZ;
    }
    
    // Then we update the voxel pointer
    if (currentPositionValid()) {
        const Point3I& v3dLowerCorner = region.getLowerCorner();
        const ssize_t  iLocalXPos = xPos - v3dLowerCorner.x;
        const ssize_t  iLocalYPos = yPos - v3dLowerCorner.y;
        const ssize_t  iLocalZPos = zPos - v3dLowerCorner.z;
        const ssize_t  uVoxelIndex = iLocalXPos + iLocalYPos * _volume->width() + iLocalZPos * _volume->width() * _volume->height();
        
        _currentVoxel = _volume->_data.begin() + uVoxelIndex;
        return true;
    }
    _currentVoxel = _volume->_data.end();
    return false;
}

void RawVolume::Sampler::movePositiveX() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    ++_posInVolume.x;
    
    if (!_volume->region().containsPointInX(_posInVolume.x)) {
        _currentPositionInvalid |= SAMPLER_INVALIDX;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDX;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        ++_currentVoxel;
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

void RawVolume::Sampler::movePositiveY() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    ++_posInVolume.y;
    
    if (!_volume->region().containsPointInY(_posInVolume.y)) {
        _currentPositionInvalid |= SAMPLER_INVALIDY;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDY;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        _currentVoxel += _volume->width();
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

void RawVolume::Sampler::movePositiveZ() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    _posInVolume.z++;
    
    if (!_volume->region().containsPointInZ(_posInVolume.z)) {
        _currentPositionInvalid |= SAMPLER_INVALIDZ;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDZ;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        _currentVoxel += _volume->width() * _volume->height();
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

void RawVolume::Sampler::moveNegativeX() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    --_posInVolume.x;
    
    if (!_volume->region().containsPointInX(_posInVolume.x)) {
        _currentPositionInvalid |= SAMPLER_INVALIDX;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDX;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        --_currentVoxel;
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

void RawVolume::Sampler::moveNegativeY() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    --_posInVolume.y;
    
    if (!_volume->region().containsPointInY(_posInVolume.y)) {
        _currentPositionInvalid |= SAMPLER_INVALIDY;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDY;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        _currentVoxel -= _volume->width();
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

void RawVolume::Sampler::moveNegativeZ() {
    const bool bIsOldPositionValid = currentPositionValid();
    
    --_posInVolume.z;
    
    if (!_volume->region().containsPointInZ(_posInVolume.z)) {
        _currentPositionInvalid |= SAMPLER_INVALIDZ;
    } else {
        _currentPositionInvalid &= ~SAMPLER_INVALIDZ;
    }
    
    // Then we update the voxel pointer
    if (!bIsOldPositionValid) {
        setPosition(_posInVolume);
    } else if (currentPositionValid()) {
        _currentVoxel -= _volume->width() * _volume->height();
    } else {
        _currentVoxel = _volume->_data.end();
    }
}

}
}
