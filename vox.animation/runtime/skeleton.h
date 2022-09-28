//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"
#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"
#include "vox.base/span.h"

namespace vox {
namespace io {
class IArchive;
class OArchive;
}  // namespace io
namespace simd_math {
struct SoaTransform;
}
namespace animation {

// Forward declaration of SkeletonBuilder, used to instantiate a skeleton.
namespace offline {
class SkeletonBuilder;
}

// This runtime skeleton data structure provides a const-only access to joint
// hierarchy, joint names and rest-pose. This structure is filled by the
// SkeletonBuilder and can be serialized/deserialized.
// Joint names, rest-poses and hierarchy information are all stored in separate
// arrays of data (as opposed to joint structures for the RawSkeleton), in order
// to closely match with the way runtime algorithms use them. Joint hierarchy is
// packed as an array of parent joint indices (16 bits), stored in depth-first
// order. This is enough to traverse the whole joint hierarchy. See
// IterateJointsDF() from skeleton_utils.h that implements a depth-first
// traversal utility.
class VOX_ANIMATION_DLL Skeleton {
public:
    // Defines Skeleton constant values.
    enum Constants {

        // Defines the maximum number of joints.
        // This is limited in order to control the number of bits required to store
        // a joint index. Limiting the number of joints also helps to handle worst
        // size cases, like when it is required to allocate an array of joints on
        // the stack.
        kMaxJoints = 1024,

        // Defines the maximum number of SoA elements required to store the maximum
        // number of joints.
        kMaxSoAJoints = (kMaxJoints + 3) / 4,

        // Defines the index of the parent of the root joint (which has no parent in
        // fact).
        kNoParent = -1,
    };

    // Builds a default skeleton.
    Skeleton();

    // Allow move.
    Skeleton(Skeleton&&) noexcept;
    Skeleton& operator=(Skeleton&&) noexcept;

    // Disables copy and assignation.
    Skeleton(Skeleton const&) = delete;
    Skeleton& operator=(Skeleton const&) = delete;

    // Declares the public non-virtual destructor.
    ~Skeleton();

    // Returns the number of joints of *this skeleton.
    [[nodiscard]] int num_joints() const { return static_cast<int>(joint_parents_.size()); }

    // Returns the number of soa elements matching the number of joints of *this
    // skeleton. This value is useful to allocate SoA runtime data structures.
    [[nodiscard]] int num_soa_joints() const { return (num_joints() + 3) / 4; }

    // Returns joint's rest poses. Rest poses are stored in soa format.
    [[nodiscard]] span<const simd_math::SoaTransform> joint_rest_poses() const { return joint_rest_poses_; }

    // Returns joint's parent indices range.
    [[nodiscard]] span<const int16_t> joint_parents() const { return joint_parents_; }

    // Returns joint's name collection.
    [[nodiscard]] span<const char* const> joint_names() const { return {joint_names_.begin(), joint_names_.end()}; }

    // Serialization functions.
    // Should not be called directly but through io::Archive << and >> operators.
    void Save(vox::io::OArchive& _archive) const;
    void Load(vox::io::IArchive& _archive, uint32_t _version);

private:
    // Internal allocation/deallocation function.
    // Allocate returns the beginning of the contiguous buffer of names.
    char* Allocate(size_t _char_count, size_t _num_joints);
    void Deallocate();

    // SkeletonBuilder class is allowed to instantiate a Skeleton.
    friend class offline::SkeletonBuilder;

    // Buffers below store joint information in joint depth first order. Their
    // size is equal to the number of joints of the skeleton.

    // Rest pose of every joint in local space.
    span<simd_math::SoaTransform> joint_rest_poses_;

    // Array of joint parent indexes.
    span<int16_t> joint_parents_;

    // Stores the name of every joint in an array of c-strings.
    span<char*> joint_names_;
};
}  // namespace animation

namespace io {
VOX_IO_TYPE_VERSION(2, animation::Skeleton)
VOX_IO_TYPE_TAG("vox-skeleton", animation::Skeleton)
}  // namespace io
}  // namespace vox