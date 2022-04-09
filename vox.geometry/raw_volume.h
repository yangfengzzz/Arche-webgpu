//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef raw_volume_hpp
#define raw_volume_hpp

#include "region.h"
#include "voxel.h"

namespace vox {
namespace geo {
/**
 * Simple volume implementation which stores data in a single large 3D array.
 *
 * This class is less memory-efficient than the PagedVolume, but it is the simplest possible
 * volume implementation which makes it useful for debugging and getting started with PolyVox.
 */
class RawVolume {
public:
    class Sampler {
    public:
        Sampler(const RawVolume& volume);
        Sampler(const RawVolume* volume);
        virtual ~Sampler();
        
        const Voxel& voxel() const;
        
        bool currentPositionValid() const;
        
        bool setPosition(const Point3I& pos);
        bool setPosition(ssize_t x, ssize_t y, ssize_t z);
        virtual bool setVoxel(const Voxel& voxel);
        const Point3I& position() const;
        
        void movePositiveX();
        void movePositiveY();
        void movePositiveZ();
        
        void moveNegativeX();
        void moveNegativeY();
        void moveNegativeZ();
        
        const Voxel& peekVoxel1nx1ny1nz() const;
        const Voxel& peekVoxel1nx1ny0pz() const;
        const Voxel& peekVoxel1nx1ny1pz() const;
        const Voxel& peekVoxel1nx0py1nz() const;
        const Voxel& peekVoxel1nx0py0pz() const;
        const Voxel& peekVoxel1nx0py1pz() const;
        const Voxel& peekVoxel1nx1py1nz() const;
        const Voxel& peekVoxel1nx1py0pz() const;
        const Voxel& peekVoxel1nx1py1pz() const;
        
        const Voxel& peekVoxel0px1ny1nz() const;
        const Voxel& peekVoxel0px1ny0pz() const;
        const Voxel& peekVoxel0px1ny1pz() const;
        const Voxel& peekVoxel0px0py1nz() const;
        const Voxel& peekVoxel0px0py0pz() const;
        const Voxel& peekVoxel0px0py1pz() const;
        const Voxel& peekVoxel0px1py1nz() const;
        const Voxel& peekVoxel0px1py0pz() const;
        const Voxel& peekVoxel0px1py1pz() const;
        
        const Voxel& peekVoxel1px1ny1nz() const;
        const Voxel& peekVoxel1px1ny0pz() const;
        const Voxel& peekVoxel1px1ny1pz() const;
        const Voxel& peekVoxel1px0py1nz() const;
        const Voxel& peekVoxel1px0py0pz() const;
        const Voxel& peekVoxel1px0py1pz() const;
        const Voxel& peekVoxel1px1py1nz() const;
        const Voxel& peekVoxel1px1py0pz() const;
        const Voxel& peekVoxel1px1py1pz() const;
        
    protected:
        RawVolume* _volume{nullptr};
        
        //The current position in the volume
        Point3I _posInVolume { 0, 0, 0 };
        
        /** Other current position information */
        std::vector<Voxel>::iterator _currentVoxel;
        
        /** Whether the current position is inside the volume */
        uint8_t _currentPositionInvalid = 0u;
    };
    
    RawVolume(const std::vector<Voxel>& data, const Region& region);
    RawVolume(std::vector<Voxel>& data, const Region& region);
    
public:
    /// Constructor for creating a fixed size volume.
    RawVolume(const Region& region);
    RawVolume(const RawVolume* copy);
    RawVolume(const RawVolume& copy);
    RawVolume(RawVolume&& move) noexcept;
    RawVolume(const RawVolume& copy, const Region& region);
    
    static RawVolume* createRaw(const std::vector<Voxel>& data, const Region& region) {
        return new RawVolume(data, region);
    }
    
    static RawVolume* createRaw(std::vector<Voxel>& data, const Region& region) {
        return new RawVolume(data, region);
    }
    
    ~RawVolume() = default;
    
    /**
     * Copy the raw data of the volume
     * @note It's the callers responsibility to properly release the memory.
     */
    std::vector<Voxel> copyVoxels() const;
    
    /**
     * The border value is returned whenever an attempt is made to read a voxel which
     * is outside the extents of the volume.
     * @return The value used for voxels outside of the volume
     */
    const Voxel& borderValue() const;
    
    /**
     * @return A Region representing the extent of the volume.
     */
    const Region& region() const;
    
    /**
     * @return The width of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the width is 64.
     * @sa height(), getDepth()
     */
    ssize_t width() const;
    /**
     * @return The height of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the height is 64.
     * @sa width(), getDepth()
     */
    ssize_t height() const;
    /**
     * @return The depth of the volume in voxels. Note that this value is inclusive, so that if the valid range is e.g. 0 to 63 then the depth is 64.
     * @sa width(), height()
     */
    ssize_t depth() const;
    
    /**
     * the vector that describes the mins value of an aabb where a voxel is set in this volume
     * deleting a voxel afterwards might lead to invalid results
     */
    Point3I mins() const;
    /**
     * the vector that describes the maxs value of an aabb where a voxel is set in this volume
     * deleting a voxel afterwards might lead to invalid results
     */
    Point3I maxs() const;
    
    /**
     * Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
     */
    const Voxel& voxel(ssize_t x, ssize_t y, ssize_t z) const;
    /**
     * @brief This version of the function is provided so that the wrap mode does not need
     * to be specified as a template parameter, as it may be confusing to some users.
     * @param pos The 3D position of the voxel
     * @return The voxel value
     */
    inline const Voxel& voxel(const Point3I& pos) const;
    
    /**
     * Sets the value used for voxels which are outside the volume
     */
    void setBorderValue(const Voxel& voxel);
    /**
     * Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
     */
    bool setVoxel(ssize_t x, ssize_t y, ssize_t z, const Voxel& voxel);
    /**
     * Sets the voxel at the position given by a 3D vector
     */
    bool setVoxel(const Point3I& pos, const Voxel& voxel);
    
    void clear();
    
    inline const uint8_t* data() const {
        return (const uint8_t*)_data.data();
    }
    
    /**
     * @brief Shift the region of the volume by the given coordinates
     */
    void translate(const Point3I& t) {
        _region.shift(t.x, t.y, t.z);
        _mins.x += t.x;
        _mins.y += t.y;
        _mins.z += t.z;
        _maxs.x += t.x;
        _maxs.y += t.y;
        _maxs.z += t.z;
    }
    
private:
    void initialise(const Region& region);
    
    /** The size of the volume */
    Region _region;
    
    /** The border value */
    Voxel _borderVoxel;
    
    /** The voxel data */
    std::vector<Voxel> _data;
    
    Point3I _mins;
    Point3I _maxs;
    bool _boundsValid;
};

inline const Region& RawVolume::region() const {
    return _region;
}

inline const Voxel& RawVolume::borderValue() const {
    return _borderVoxel;
}

inline ssize_t RawVolume::width() const {
    return _region.getWidthInVoxels();
}

inline ssize_t RawVolume::height() const {
    return _region.getHeightInVoxels();
}

inline ssize_t RawVolume::depth() const {
    return _region.getDepthInVoxels();
}

inline Point3I RawVolume::mins() const {
    if (!_boundsValid) {
        return _region.getLowerCorner();
    }
    return _mins;
}

inline Point3I RawVolume::maxs() const {
    if (!_boundsValid) {
        return _region.getUpperCorner();
    }
    return _maxs;
}

inline const Voxel& RawVolume::voxel(const Point3I& pos) const {
    return voxel(pos.x, pos.y, pos.z);
}

#define CAN_GO_NEG_X(val) ((val) > region.getLowerX())
#define CAN_GO_POS_X(val) ((val) < region.getUpperX())
#define CAN_GO_NEG_Y(val) ((val) > region.getLowerY())
#define CAN_GO_POS_Y(val) ((val) < region.getUpperY())
#define CAN_GO_NEG_Z(val) ((val) > region.getLowerZ())
#define CAN_GO_POS_Z(val) ((val) < region.getUpperZ())

inline const Point3I& RawVolume::Sampler::position() const {
    return _posInVolume;
}

inline const Voxel& RawVolume::Sampler::voxel() const {
    if (this->currentPositionValid()) {
        return *_currentVoxel;
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y, this->_posInVolume.z);
}

inline bool RawVolume::Sampler::currentPositionValid() const {
    return !_currentPositionInvalid;
}

inline bool RawVolume::Sampler::setPosition(const Point3I& v3dNewPos) {
    return setPosition(v3dNewPos.x, v3dNewPos.y, v3dNewPos.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1ny1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 - region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y - 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1ny0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y)) {
        return *(_currentVoxel - 1 - region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y - 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1ny1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 - region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y - 1, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx0py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx0py0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x)) {
        return *(_currentVoxel - 1);
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx0py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 + region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y + 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1py0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y)) {
        return *(_currentVoxel - 1 + region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y + 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1nx1py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - 1 + region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x - 1, this->_posInVolume.y + 1, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1ny1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y - 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1ny0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_Y(this->_posInVolume.y)) {
        return *(_currentVoxel - region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y - 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1ny1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y - 1, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px0py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px0py0pz() const {
    if (this->currentPositionValid()) {
        return *_currentVoxel;
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px0py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y + 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1py0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_Y(this->_posInVolume.y)) {
        return *(_currentVoxel + region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y + 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel0px1py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x, this->_posInVolume.y + 1, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1ny1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 - region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y - 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1ny0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y)) {
        return *(_currentVoxel + 1 - region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y - 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1ny1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_NEG_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 - region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y - 1, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px0py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px0py0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x)) {
        return *(_currentVoxel + 1);
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px0py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y, this->_posInVolume.z + 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1py1nz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_NEG_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 + region.getWidthInVoxels() - region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y + 1, this->_posInVolume.z - 1);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1py0pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y)) {
        return *(_currentVoxel + 1 + region.getWidthInVoxels());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y + 1, this->_posInVolume.z);
}

inline const Voxel& RawVolume::Sampler::peekVoxel1px1py1pz() const {
    const Region& region = this->_volume->region();
    if (this->currentPositionValid() && CAN_GO_POS_X(this->_posInVolume.x) && CAN_GO_POS_Y(this->_posInVolume.y) && CAN_GO_POS_Z(this->_posInVolume.z)) {
        return *(_currentVoxel + 1 + region.getWidthInVoxels() + region.stride());
    }
    return this->_volume->voxel(this->_posInVolume.x + 1, this->_posInVolume.y + 1, this->_posInVolume.z + 1);
}

#undef CAN_GO_NEG_X
#undef CAN_GO_POS_X
#undef CAN_GO_NEG_Y
#undef CAN_GO_POS_Y
#undef CAN_GO_NEG_Z
#undef CAN_GO_POS_Z

}
}
#endif /* raw_volume_hpp */
