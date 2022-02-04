//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "animator_loader.h"
#include <glog/logging.h>
#include <ozz/base/io/archive.h>

namespace ozz {
namespace loader {
bool LoadSkeleton(const char* _filename, animation::Skeleton* _skeleton) {
    assert(_filename && _skeleton);
    LOG(INFO) << "Loading skeleton archive " << _filename << "."
    << std::endl;
    io::File file(_filename, "rb");
    if (!file.opened()) {
        LOG(ERROR) << "Failed to open skeleton file " << _filename << "."
        << std::endl;
        return false;
    }
    io::IArchive archive(&file);
    if (!archive.TestTag<animation::Skeleton>()) {
        LOG(ERROR) << "Failed to load skeleton instance from file "
        << _filename << "." << std::endl;
        return false;
    }
    
    // Once the tag is validated, reading cannot fail.
    archive >> *_skeleton;
    
    return true;
}

bool LoadAnimation(const char* _filename, animation::Animation* _animation) {
    assert(_filename && _animation);
    LOG(INFO) << "Loading animation archive: " << _filename << "."
    << std::endl;
    io::File file(_filename, "rb");
    if (!file.opened()) {
        LOG(ERROR) << "Failed to open animation file " << _filename << "."
        << std::endl;
        return false;
    }
    io::IArchive archive(&file);
    if (!archive.TestTag<animation::Animation>()) {
        LOG(ERROR) << "Failed to load animation instance from file "
        << _filename << "." << std::endl;
        return false;
    }
    
    // Once the tag is validated, reading cannot fail.
    archive >> *_animation;
    
    return true;
}

namespace {
template <typename _Track>
bool LoadTrackImpl(const char* _filename, _Track* _track) {
    assert(_filename && _track);
    LOG(INFO) << "Loading track archive: " << _filename << "." << std::endl;
    io::File file(_filename, "rb");
    if (!file.opened()) {
        LOG(ERROR) << "Failed to open track file " << _filename << "."
        << std::endl;
        return false;
    }
    io::IArchive archive(&file);
    if (!archive.TestTag<_Track>()) {
        LOG(ERROR) << "Failed to load float track instance from file "
        << _filename << "." << std::endl;
        return false;
    }
    
    // Once the tag is validated, reading cannot fail.
    archive >> *_track;
    
    return true;
}
}  // namespace

bool LoadTrack(const char* _filename, animation::FloatTrack* _track) {
    return LoadTrackImpl(_filename, _track);
}

bool LoadTrack(const char* _filename, animation::Float2Track* _track) {
    return LoadTrackImpl(_filename, _track);
}

bool LoadTrack(const char* _filename, animation::Float3Track* _track) {
    return LoadTrackImpl(_filename, _track);
}

bool LoadTrack(const char* _filename, animation::Float4Track* _track) {
    return LoadTrackImpl(_filename, _track);
}

bool LoadTrack(const char* _filename, animation::QuaternionTrack* _track) {
    return LoadTrackImpl(_filename, _track);
}

} // loader
} // ozz
