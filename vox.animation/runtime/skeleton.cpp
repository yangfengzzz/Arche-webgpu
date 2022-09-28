//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/skeleton.h"

#include <cstring>

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.base/memory/allocator.h"
#include "vox.simd_math/soa_math_archive.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::animation {

Skeleton::Skeleton() = default;

Skeleton::Skeleton(Skeleton&& _other) noexcept { *this = std::move(_other); }

Skeleton& Skeleton::operator=(Skeleton&& _other) noexcept {
    std::swap(joint_rest_poses_, _other.joint_rest_poses_);
    std::swap(joint_parents_, _other.joint_parents_);
    std::swap(joint_names_, _other.joint_names_);

    return *this;
}

Skeleton::~Skeleton() { Deallocate(); }

char* Skeleton::Allocate(size_t _chars_size, size_t _num_joints) {
    // Distributes buffer memory while ensuring proper alignment (serves larger
    // alignment values first).
    static_assert(alignof(simd_math::SoaTransform) >= alignof(char*) && alignof(char*) >= alignof(int16_t) &&
                          alignof(int16_t) >= alignof(char),
                  "Must serve larger alignment values first)");

    assert(joint_rest_poses_.empty() && joint_names_.empty() && joint_parents_.empty());

    // Early out if no joint.
    if (_num_joints == 0) {
        return nullptr;
    }

    // Rest poses have SoA format
    const size_t num_soa_joints = (_num_joints + 3) / 4;
    const size_t joint_rest_poses_size = num_soa_joints * sizeof(simd_math::SoaTransform);
    const size_t names_size = _num_joints * sizeof(char*);
    const size_t joint_parents_size = _num_joints * sizeof(int16_t);
    const size_t buffer_size = names_size + _chars_size + joint_parents_size + joint_rest_poses_size;

    // Allocates whole buffer.
    span<byte> buffer = {
            static_cast<byte*>(memory::default_allocator()->Allocate(buffer_size, alignof(simd_math::SoaTransform))),
            buffer_size};

    // Serves larger alignment values first.
    // Rest pose first, biggest alignment.
    joint_rest_poses_ = fill_span<simd_math::SoaTransform>(buffer, num_soa_joints);

    // Then names array, second-biggest alignment.
    joint_names_ = fill_span<char*>(buffer, _num_joints);

    // Parents, third-biggest alignment.
    joint_parents_ = fill_span<int16_t>(buffer, _num_joints);

    // Remaining buffer will be used to store joint names.
    assert(buffer.size_bytes() == _chars_size && "Whole buffer should be consumed");
    return reinterpret_cast<char*>(buffer.data());
}

void Skeleton::Deallocate() {
    memory::default_allocator()->Deallocate(as_writable_bytes(joint_rest_poses_).data());
    joint_rest_poses_ = {};
    joint_names_ = {};
    joint_parents_ = {};
}

void Skeleton::Save(vox::io::OArchive& _archive) const {
    const int32_t num_joints = this->num_joints();

    // Early out if skeleton's empty.
    _archive << num_joints;
    if (!num_joints) {
        return;
    }

    // Stores names. They are all concatenated in the same buffer, starting at
    // joint_names_[0].
    size_t chars_count = 0;
    for (int i = 0; i < num_joints; ++i) {
        chars_count += (std::strlen(joint_names_[i]) + 1) * sizeof(char);
    }
    _archive << static_cast<int32_t>(chars_count);
    _archive << vox::io::MakeArray(joint_names_[0], chars_count);
    _archive << vox::io::MakeArray(joint_parents_);
    _archive << vox::io::MakeArray(joint_rest_poses_);
}

void Skeleton::Load(vox::io::IArchive& _archive, uint32_t _version) {
    // Deallocate skeleton in case it was already used before.
    Deallocate();

    if (_version != 2) {
        LOGE("Unsupported Skeleton version {}", _version)
        return;
    }

    int32_t num_joints;
    _archive >> num_joints;

    // Early out if skeleton's empty.
    if (!num_joints) {
        return;
    }

    // Read names.
    int32_t chars_count;
    _archive >> chars_count;

    // Allocates all skeleton data members.
    char* cursor = Allocate(chars_count, num_joints);

    // Reads name's buffer, they are all contiguous in the same buffer.
    _archive >> vox::io::MakeArray(cursor, chars_count);

    // Fixes up array of pointers. Stops at num_joints - 1, so that it doesn't
    // read memory past the end of the buffer.
    for (int i = 0; i < num_joints - 1; ++i) {
        joint_names_[i] = cursor;
        cursor += std::strlen(joint_names_[i]) + 1;
    }
    // num_joints is > 0, as this was tested at the beginning of the function.
    joint_names_[num_joints - 1] = cursor;

    _archive >> vox::io::MakeArray(joint_parents_);
    _archive >> vox::io::MakeArray(joint_rest_poses_);
}
}  // namespace vox::animation
