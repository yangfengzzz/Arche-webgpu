//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/vector.h"
#include "vox.base/io/archive_traits.h"
#include "vox.math/scalable_transform.h"

namespace vox {
namespace animation::offline {

// Off-line skeleton type.
// This skeleton type is not intended to be used in run time. It is used to
// define the offline skeleton object that can be converted to the runtime
// skeleton using the SkeletonBuilder. This skeleton structure exposes joints'
// hierarchy. A joint is defined with a name, a transformation (its rest_pose
// pose), and its children. Children are exposed as a public std::vector of
// joints. This same type is used for skeleton roots, also exposed from the
// public API. The public API exposed through std:vector's of joints can be used
// freely with the only restriction that the total number of joints does not
// exceed Skeleton::kMaxJoints.
struct VOX_ANIMOFFLINE_DLL RawSkeleton {
    // Construct an empty skeleton.
    RawSkeleton();

    // The destructor is responsible for deleting the roots and their hierarchy.
    ~RawSkeleton();

    // Offline skeleton joint type.
    struct Joint {
        // Type of the list of children joints.
        typedef vox::vector<Joint> Children;

        // Children joints.
        Children children;

        // The name of the joint.
        vox::string name;

        // Joint rest pose transformation in local space.
        ScalableTransform transform;
    };

    // Tests for *this validity.
    // Returns true on success or false on failure if the number of joints exceeds
    // vox::Skeleton::kMaxJoints.
    [[nodiscard]] bool Validate() const;

    // Returns the number of joints of *this animation.
    // This function is not constant time as it iterates the hierarchy of joints
    // and counts them.
    [[nodiscard]] int num_joints() const;

    // Declares the skeleton's roots. Can be empty if the skeleton has no joint.
    Joint::Children roots;
};

namespace {
// Internal function used to iterate through joint hierarchy depth-first.
template <typename Fct>
inline void iterHierarchyRecurseDF(const RawSkeleton::Joint::Children& _children,
                                   const RawSkeleton::Joint* _parent,
                                   Fct& _fct) {
    for (const auto& current : _children) {
        _fct(current, _parent);
        iterHierarchyRecurseDF(current.children, &current, _fct);
    }
}

// Internal function used to iterate through joint hierarchy breadth-first.
template <typename Fct>
inline void iterHierarchyRecurseBF(const RawSkeleton::Joint::Children& _children,
                                   const RawSkeleton::Joint* _parent,
                                   Fct& _fct) {
    for (const auto& current : _children) {
        _fct(current, _parent);
    }
    for (const auto& current : _children) {
        iterHierarchyRecurseBF(current.children, &current, _fct);
    }
}
}  // namespace

// Applies a specified functor to each joint in a depth-first order.
// _Fct is of type void(const Joint& _current, const Joint* _parent) where the
// first argument is the child of the second argument. _parent is null if the
// _current joint is the root.
template <typename Fct>
inline Fct IterateJointsDF(const RawSkeleton& _skeleton, Fct _fct) {
    iterHierarchyRecurseDF(_skeleton.roots, nullptr, _fct);
    return _fct;
}

// Applies a specified functor to each joint in a breadth-first order.
// _Fct is of type void(const Joint& _current, const Joint* _parent) where the
// first argument is the child of the second argument. _parent is null if the
// _current joint is the root.
template <typename Fct>
inline Fct IterateJointsBF(const RawSkeleton& _skeleton, Fct _fct) {
    iterHierarchyRecurseBF(_skeleton.roots, nullptr, _fct);
    return _fct;
}
}  // namespace animation::offline
namespace io {
VOX_IO_TYPE_VERSION(1, animation::offline::RawSkeleton)
VOX_IO_TYPE_TAG("ozz-raw_skeleton", animation::offline::RawSkeleton)

// Should not be called directly but through io::Archive << and >> operators.
template <>
struct VOX_ANIMOFFLINE_DLL Extern<animation::offline::RawSkeleton> {
    static void Save(OArchive& _archive, const animation::offline::RawSkeleton* _skeletons, size_t _count);
    static void Load(IArchive& _archive, animation::offline::RawSkeleton* _skeletons, size_t _count, uint32_t _version);
};
}  // namespace io
}  // namespace vox