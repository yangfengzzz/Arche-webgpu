//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef raw_volume_wrapper_h
#define raw_volume_wrapper_h

#include "raw_volume.h"

namespace vox {
namespace geo {
/**
 * @brief A wrapper for a RawVolume that performs a sanity check for the setVoxel call.
 */
class RawVolumeWrapper {
private:
    RawVolume* _volume;
    Region _region;
    Region _dirtyRegion = Region::InvalidRegion;
    
public:
    class Sampler : public RawVolume::Sampler {
    private:
        using Super = RawVolume::Sampler;
        RawVolumeWrapper* _volume = nullptr;
    public:
        Sampler(const RawVolumeWrapper* volume) : Super(volume->volume()) {}
        
        Sampler(const RawVolumeWrapper& volume) : Super(volume.volume()) {};
        
        Sampler(RawVolumeWrapper* volume) : Super(volume->volume()), _volume(volume) {}
        
        Sampler(RawVolumeWrapper& volume) : Super(volume.volume()), _volume(&volume) {};
        
        bool setVoxel(const Voxel& voxel) override {
            if (Super::setVoxel(voxel)) {
                core_assert(_volume);
                if (!_volume) {
                    return true;
                }
                if (_volume->_dirtyRegion.isValid()) {
                    _volume->_dirtyRegion.accumulate(position());
                } else {
                    _volume->_dirtyRegion = Region(position(), position());
                }
                return true;
            }
            return false;
        }
    };
    
    RawVolumeWrapper(voxel::RawVolume* volume) :
    _volume(volume), _region(volume->region()) {
    }
    
    inline operator RawVolume& () const {
        return *_volume;
    }
    
    inline operator const RawVolume& () const {
        return *_volume;
    }
    
    inline operator RawVolume* () const {
        return _volume;
    }
    
    inline operator const RawVolume* () const {
        return _volume;
    }
    
    inline RawVolume* volume() const {
        return _volume;
    }
    
    inline void setVolume(RawVolume* v) {
        _volume = v;
    }
    
    inline const Region& region() const {
        return _region;
    }
    
    void setRegion(const Region& region) {
        _region.cropTo(region);
    }
    
    inline const Voxel& voxel(const Point3I& pos) const {
        return _volume->voxel(pos.x, pos.y, pos.z);
    }
    
    inline const Voxel& voxel(int x, int y, int z) const {
        return _volume->voxel(x, y, z);
    }
    
    inline bool setVoxel(const Point3I& pos, const Voxel& voxel) {
        return setVoxel(pos.x, pos.y, pos.z, voxel);
    }
    
    inline const Region& dirtyRegion() const {
        return _dirtyRegion;
    }
    
    /**
     * @return @c false if the voxel was not placed because the given position is outside of the valid region, @c
     * true if the voxel was placed in the region.
     * @note The return values have a different meaning as for the wrapped RawVolume.
     */
    inline bool setVoxel(int x, int y, int z, const Voxel& voxel) {
        const Point3I p(x, y, z);
        if (!_region.containsPoint(p)) {
            return false;
        }
        if (_volume->setVoxel(p, voxel)) {
            if (_dirtyRegion.isValid()) {
                _dirtyRegion.accumulate(p);
            } else {
                _dirtyRegion = Region(p, p);
            }
        }
        return true;
    }
    
    void translate(const Point3I& t) {
        _volume->translate(t);
        _dirtyRegion.shift(t.x, t.y, t.z);
    }
    
    inline bool setVoxels(int x, int z, const Voxel* voxels, int amount) {
        for (int y = 0; y < amount; ++y) {
            setVoxel(x, y, z, voxels[y]);
        }
        return true;
    }
    
    inline bool setVoxels(int x, int y, int z, int nx, int nz, const Voxel* voxels, int amount) {
        for (int j = 0; j < nx; ++j) {
            for (int k = 0; k < nz; ++k) {
                for (int ny = 0; ny < amount; ++ny) {
                    setVoxel(x + j, y + ny, z + k, voxels[ny]);
                }
            }
        }
        return true;
    }
};

}
}
#endif /* raw_volume_wrapper_h */
