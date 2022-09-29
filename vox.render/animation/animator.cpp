//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.simd_math/soa_transform.h"

namespace vox {
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

    _models.resize(_skeleton.num_joints());
    _ltm_job.output = make_span(_models);
    _ltm_job.skeleton = &_skeleton;
    rootState->loadSkeleton(_skeleton);
    return true;
}

void Animator::update(float dt) {
    rootState->update(dt);
    _ltm_job.input = make_span(rootState->locals());
    _ltm_job.Run();
}

bool Animator::localToModelFromExcluded() const {
    return _ltm_job.from_excluded;
}

void Animator::setLocalToModelFromExcluded(bool value) {
    _ltm_job.from_excluded = value;
}

int Animator::localToModelFrom() const {
    return _ltm_job.from;
}

void Animator::setLocalToModelFrom(int value) {
    _ltm_job.from = value;
}

int Animator::localToModelTo() const {
    return _ltm_job.to;
}

void Animator::setLocalToModelTo(int value) {
    _ltm_job.to = value;
}

const vox::vector<simd_math::Float4x4>& Animator::models() const {
    return _models;
}

}  // namespace vox