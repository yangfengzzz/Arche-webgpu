//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"

namespace vox {
void Animator::update(float dt) {
    if (blending) {
        blending->update(dt);
    }
    _ltm_job.Run();
}

bool Animator::loadSkeleton(const char* _filename) {
    assert(_filename);
    LOGI("Loading skeleton archive {}", _filename)
    vox::io::File file(_filename, "rb");
    if (!file.opened()) {
        LOGE("Failed to open skeleton file {}", _filename)
        return false;
    }
    vox::io::IArchive archive(&file);
    if (!archive.TestTag<vox::animation::Skeleton>()) {
        LOGE("Failed to load skeleton instance from file {}.", _filename)
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> _skeleton;

    return true;
}

}  // namespace vox