//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/components_manager.h"

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
    (void)_ltm_job.Run();
}

bool Animator::localToModelFromExcluded() const { return _ltm_job.from_excluded; }

void Animator::setLocalToModelFromExcluded(bool value) { _ltm_job.from_excluded = value; }

int Animator::localToModelFrom() const { return _ltm_job.from; }

void Animator::setLocalToModelFrom(int value) { _ltm_job.from = value; }

int Animator::localToModelTo() const { return _ltm_job.to; }

void Animator::setLocalToModelTo(int value) { _ltm_job.to = value; }

const vox::vector<simd_math::Float4x4>& Animator::models() const { return _models; }

void Animator::_multiplySoATransformQuaternion(int _index,
                                               const simd_math::SimdQuaternion& _quat,
                                               const span<simd_math::SoaTransform>& _transforms) {
    assert(_index >= 0 && static_cast<size_t>(_index) < _transforms.size() * 4 && "joint index out of bound.");

    // Convert soa to aos in order to perform quaternion multiplication, and gets
    // back to soa.
    vox::simd_math::SoaTransform& soa_transform_ref = _transforms[_index / 4];
    vox::simd_math::SimdQuaternion aos_quats[4];
    vox::simd_math::Transpose4x4(&soa_transform_ref.rotation.x, &aos_quats->xyzw);

    vox::simd_math::SimdQuaternion& aos_quat_ref = aos_quats[_index & 3];
    aos_quat_ref = aos_quat_ref * _quat;

    vox::simd_math::Transpose4x4(&aos_quats->xyzw, &soa_transform_ref.rotation.x);
}

void Animator::computeSkeletonBounds(BoundingBox3F& bound) {
    using vox::simd_math::Float4x4;

    // Set a default box.
    bound = BoundingBox3F();

    const int num_joints = _skeleton.num_joints();
    if (!num_joints) {
        return;
    }

    // Allocate matrix array, out of memory is handled by the LocalToModelJob.
    vox::vector<vox::simd_math::Float4x4> models(num_joints);

    // Compute model space rest pose.
    vox::animation::LocalToModelJob job;
    job.input = _skeleton.joint_rest_poses();
    job.output = make_span(models);
    job.skeleton = &_skeleton;
    if (job.Run()) {
        // Forwards to posture function.
        _computePostureBounds(job.output, &bound);
    }
}

void Animator::_computePostureBounds(span<const simd_math::Float4x4> _matrices, BoundingBox3F* _bound) {
    assert(_bound);

    // Set a default box.
    *_bound = BoundingBox3F();

    if (_matrices.empty()) {
        return;
    }

    // Loops through matrices and stores min/max.
    // Matrices array cannot be empty, it was checked at the beginning of the
    // function.
    const vox::simd_math::Float4x4* current = _matrices.begin();
    simd_math::SimdFloat4 min = current->cols[3];
    simd_math::SimdFloat4 max = current->cols[3];
    ++current;
    while (current < _matrices.end()) {
        min = simd_math::Min(min, current->cols[3]);
        max = simd_math::Max(max, current->cols[3]);
        ++current;
    }

    // Stores in math::Box structure.
    simd_math::Store3PtrU(min, &_bound->lower_corner.x);
    simd_math::Store3PtrU(max, &_bound->upper_corner.x);
}

void Animator::_onEnable() { ComponentsManager::GetSingleton().addOnUpdateAnimators(this); }

void Animator::_onDisable() { ComponentsManager::GetSingleton().removeOnUpdateAnimators(this); }

}  // namespace vox