//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "paged_volume_wrapper.h"
#include "morton.h"

namespace vox {
namespace geo {
PagedVolumeWrapper::Sampler::Sampler(const PagedVolumeWrapper* volume) :
Super(volume->volume()), _chunk(volume->_chunk) {
}

PagedVolumeWrapper::Sampler::Sampler(const PagedVolumeWrapper& volume) :
Super(volume.volume()), _chunk(volume._chunk) {
}

void PagedVolumeWrapper::Sampler::setPosition(ssize_t xPos, ssize_t yPos, ssize_t zPos) {
    _xPosInVolume = xPos;
    _yPosInVolume = yPos;
    _zPosInVolume = zPos;
    
    // Then we update the voxel pointer
    const ssize_t xChunk = _xPosInVolume >> _volume->_chunkSideLengthPower;
    const ssize_t yChunk = _yPosInVolume >> _volume->_chunkSideLengthPower;
    const ssize_t zChunk = _zPosInVolume >> _volume->_chunkSideLengthPower;
    
    _xPosInChunk = static_cast<uint16_t>(_xPosInVolume - (xChunk << _volume->_chunkSideLengthPower));
    _yPosInChunk = static_cast<uint16_t>(_yPosInVolume - (yChunk << _volume->_chunkSideLengthPower));
    _zPosInChunk = static_cast<uint16_t>(_zPosInVolume - (zChunk << _volume->_chunkSideLengthPower));
    const uint32_t voxelIndexInChunk = morton256_x[_xPosInChunk] | morton256_y[_yPosInChunk] | morton256_z[_zPosInChunk];
    
    const Point3I& p = _chunk->_chunkSpacePosition;
    if (p.x == xChunk && p.y == yChunk && p.z == zChunk) {
        _currentChunk = _chunk;
    } else {
        _currentChunk = _volume->chunk(xChunk, yChunk, zChunk);
    }
    
    _currentVoxel = _currentChunk->_data + voxelIndexInChunk;
}

PagedVolumeWrapper::PagedVolumeWrapper(PagedVolume* voxelStorage,
                                       const PagedVolume::ChunkPtr& chunk, const Region& region) :
_pagedVolume(voxelStorage), _chunk(std::move(chunk)), _region(region) {
    if (_chunk != nullptr) {
        const ssize_t sideLength = (ssize_t)voxelStorage->chunkSideLength();
        const Point3I& mins = chunk->chunkPos() * sideLength;
        const Point3I maxs = Point3I(mins.x + sideLength - 1,
                                     mins.y + sideLength - 1,
                                     mins.z + sideLength - 1);
        _validRegion = Region(mins, maxs);
    }
}

const Voxel& PagedVolumeWrapper::voxel(ssize_t x, ssize_t y, ssize_t z) const {
    if (_validRegion.containsPoint(x, y, z)) {
        assert(_chunk != nullptr);
        const ssize_t relX = x - _validRegion.getLowerX();
        const ssize_t relY = y - _validRegion.getLowerY();
        const ssize_t relZ = z - _validRegion.getLowerZ();
        return _chunk->voxel(relX, relY, relZ);
    }
    assert(_pagedVolume != nullptr);
    return _pagedVolume->voxel(x, y, z);
}

bool PagedVolumeWrapper::setVoxel(ssize_t x, ssize_t y, ssize_t z, const Voxel& voxel) {
    // core_trace_scoped(SetVoxel);
    if (_validRegion.containsPoint(x, y, z)) {
        assert(_chunk != nullptr);
        const ssize_t relX = x - _validRegion.getLowerX();
        const ssize_t relY = y - _validRegion.getLowerY();
        const ssize_t relZ = z - _validRegion.getLowerZ();
        _chunk->setVoxel(relX, relY, relZ, voxel);
        return true;
    }
    assert(_pagedVolume != nullptr);
    _pagedVolume->setVoxel(x, y, z, voxel);
    return true;
}

bool PagedVolumeWrapper::setVoxels(ssize_t x, ssize_t y, ssize_t z,
                                   ssize_t nx, ssize_t nz, const Voxel* voxels, ssize_t amount) {
    // core_trace_scoped(WrapperSetVoxels);
    for (ssize_t j = 0; j < nx; ++j) {
        for (ssize_t k = 0; k < nz; ++k) {
            const ssize_t fx = x + j;
            const ssize_t fz = z + k;
            ssize_t left = amount;
            if (_validRegion.containsPoint(fx, y, fz)) {
                // first part goes into the chunk
                const ssize_t h = _validRegion.getHeightInVoxels();
                _chunk->setVoxels(fx - _validRegion.getLowerX(), y - _validRegion.getLowerY(),
                                  fz - _validRegion.getLowerZ(), voxels, std::min(h, left));
                left -= h;
                if (left > 0) {
                    // everything else goes into the volume
                    assert(_pagedVolume != nullptr);
                    _pagedVolume->setVoxels(fx, y + h, fz, 1, 1, voxels + h, left);
                }
            } else {
                // TODO: add region/chunk support here, too
                assert(_pagedVolume != nullptr);
                _pagedVolume->setVoxels(fx, y, fz, 1, 1, voxels, left);
            }
        }
    }
    return true;
}

}
}
