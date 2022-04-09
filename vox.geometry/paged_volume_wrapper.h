//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef paged_volume_wrapper_hpp
#define paged_volume_wrapper_hpp

#include "paged_volume.h"

namespace vox {
namespace geo {
/**
 * @brief Wrapper around a PagedVolume that reduces the amount of needed locks.
 */
class PagedVolumeWrapper {
private:
    PagedVolume* _pagedVolume;
    PagedVolume::ChunkPtr _chunk;
    Region _validRegion;
    Region _region;
    
public:
    class Sampler : public PagedVolume::Sampler {
    private:
        using Super = PagedVolume::Sampler;
        PagedVolume::ChunkPtr _chunk;
    public:
        Sampler(const PagedVolumeWrapper* volume);
        Sampler(const PagedVolumeWrapper& volume);
        
        void setPosition(ssize_t xPos, ssize_t yPos, ssize_t zPos) override;
    };
    
    PagedVolumeWrapper(PagedVolume* voxelStorage, const PagedVolume::ChunkPtr& chunk, const Region& region);
    virtual ~PagedVolumeWrapper() {}
    
    operator PagedVolume& () const;
    operator const PagedVolume& () const;
    operator PagedVolume* () const;
    operator const PagedVolume* () const;
    
    PagedVolume::ChunkPtr chunk() const;
    PagedVolume* volume() const;
    const Region& region() const;
    
    const Voxel& voxel(const Point3I& pos) const;
    const Voxel& voxel(ssize_t x, ssize_t y, ssize_t z) const;
    
    bool setVoxel(const Point3I& pos, const Voxel& voxel);
    bool setVoxel(ssize_t x, ssize_t y, ssize_t z, const Voxel& voxel);
    bool setVoxels(ssize_t x, ssize_t z, const Voxel* voxels, ssize_t amount);
    bool setVoxels(ssize_t x, ssize_t y, ssize_t z, ssize_t nx, ssize_t nz, const Voxel* voxels, ssize_t amount);
    bool setVoxels(ssize_t x, ssize_t y, ssize_t z, const Voxel* voxels, ssize_t amount);
};

inline const Region& PagedVolumeWrapper::region() const {
    return _region;
}

inline PagedVolumeWrapper::operator PagedVolume& () const {
    return *_pagedVolume;
}

inline PagedVolumeWrapper::operator const PagedVolume& () const {
    return *_pagedVolume;
}

inline PagedVolumeWrapper::operator PagedVolume* () const {
    return _pagedVolume;
}

inline PagedVolumeWrapper::operator const PagedVolume* () const {
    return _pagedVolume;
}

inline PagedVolume::ChunkPtr PagedVolumeWrapper::chunk() const {
    return _chunk;
}

inline PagedVolume* PagedVolumeWrapper::volume() const {
    return _pagedVolume;
}

inline bool PagedVolumeWrapper::setVoxel(const Point3I& pos, const Voxel& voxel) {
    return setVoxel(pos.x, pos.y, pos.z, voxel);
}

inline const Voxel& PagedVolumeWrapper::voxel(const Point3I& pos) const {
    return voxel(pos.x, pos.y, pos.z);
}

inline bool PagedVolumeWrapper::setVoxels(ssize_t x, ssize_t z, const Voxel* voxels, ssize_t amount) {
    return setVoxels(x, 0, z, 1, 1, voxels, amount);
}

inline bool PagedVolumeWrapper::setVoxels(ssize_t x, ssize_t y, ssize_t z, const Voxel* voxels, ssize_t amount) {
    return setVoxels(x, y, z, 1, 1, voxels, amount);
}

}
}
#endif /* paged_volume_wrapper_hpp */
