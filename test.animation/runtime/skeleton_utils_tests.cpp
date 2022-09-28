//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <algorithm>

#include "gtest/gtest.h"
#include "test.animation/gtest_helper.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.animation/offline/skeleton_builder.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.animation/runtime/skeleton_utils.h"
#include "vox.base/memory/unique_ptr.h"

using vox::animation::Skeleton;
using vox::animation::offline::RawSkeleton;
using vox::animation::offline::SkeletonBuilder;

TEST(JointRestPose, SkeletonUtils) {
    // Instantiates a builder objects with default parameters.
    SkeletonBuilder builder;

    RawSkeleton raw_skeleton;
    raw_skeleton.roots.resize(1);
    RawSkeleton::Joint& r = raw_skeleton.roots[0];
    r.name = "r0";
    r.transform.translation = vox::Vector3F::x_axis();
    r.transform.rotation = vox::QuaternionF::makeIdentity();
    r.transform.scale = vox::Vector3F::zero();

    r.children.resize(2);
    RawSkeleton::Joint& c0 = r.children[0];
    c0.name = "j0";
    c0.transform.translation = vox::Vector3F::y_axis();
    c0.transform.rotation = -vox::QuaternionF::makeIdentity();
    c0.transform.scale = -vox::Vector3F::one();

    RawSkeleton::Joint& c1 = r.children[1];
    c1.name = "j1";
    c1.transform.translation = vox::Vector3F::z_axis();
    c1.transform.rotation = vox::QuaternionF::makeIdentity().conjugate();
    c1.transform.scale = vox::Vector3F::one();

    EXPECT_TRUE(raw_skeleton.Validate());
    EXPECT_EQ(raw_skeleton.num_joints(), 3);

    vox::unique_ptr<Skeleton> skeleton(builder(raw_skeleton));
    ASSERT_TRUE(skeleton);
    EXPECT_EQ(skeleton->num_joints(), 3);

    // Out of range.
    EXPECT_ASSERTION(GetJointLocalRestPose(*skeleton, 3), "Joint index out of range.");

    const vox::ScalableTransform rest_pose0 = GetJointLocalRestPose(*skeleton, 0);
    EXPECT_FLOAT3_EQ(rest_pose0.translation, 1.f, 0.f, 0.f);
    EXPECT_QUATERNION_EQ(rest_pose0.rotation, 0.f, 0.f, 0.f, 1.f);
    EXPECT_FLOAT3_EQ(rest_pose0.scale, 0.f, 0.f, 0.f);

    const vox::ScalableTransform rest_pose1 = GetJointLocalRestPose(*skeleton, 1);
    EXPECT_FLOAT3_EQ(rest_pose1.translation, 0.f, 1.f, 0.f);
    EXPECT_QUATERNION_EQ(rest_pose1.rotation, 0.f, 0.f, 0.f, -1.f);
    EXPECT_FLOAT3_EQ(rest_pose1.scale, -1.f, -1.f, -1.f);

    const vox::ScalableTransform rest_pose2 = GetJointLocalRestPose(*skeleton, 2);
    EXPECT_FLOAT3_EQ(rest_pose2.translation, 0.f, 0.f, 1.f);
    EXPECT_QUATERNION_EQ(rest_pose2.rotation, -0.f, -0.f, -0.f, 1.f);
    EXPECT_FLOAT3_EQ(rest_pose2.scale, 1.f, 1.f, 1.f);
}

/* Definition of the skeleton used by the tests.
 10 joints, 2 roots

      *
    /   \
   j0    j8
 /   \     \
 j1   j4    j9
 |   / \
 j2 j5 j6
 |     |
 j3    j7
 */

namespace {

class IterateDFFailTester {
public:
    void operator()(int, int) { ASSERT_TRUE(false); }
};

class IterateDFTester {
public:
    IterateDFTester(const vox::animation::Skeleton* _skeleton, int _start)
        : skeleton_(_skeleton), start_(_start), num_iterations_(0) {}

    void operator()(int _current, int _parent) {
        const int joint = start_ + num_iterations_;
        EXPECT_EQ(joint, _current);
        EXPECT_EQ(skeleton_->joint_parents()[joint], _parent);
        ++num_iterations_;
    }

    [[nodiscard]] int num_iterations() const { return num_iterations_; }

private:
    // Iterated skeleton.
    const vox::animation::Skeleton* skeleton_;

    // First joint to explore.
    int start_;

    // Number of iterations completed.
    int num_iterations_;
};
}  // namespace

TEST(InterateDF, SkeletonUtils) {
    // Instantiates a builder objects with default parameters.
    SkeletonBuilder builder;

    RawSkeleton raw_skeleton;
    raw_skeleton.roots.resize(2);
    RawSkeleton::Joint& j0 = raw_skeleton.roots[0];
    j0.name = "j0";
    RawSkeleton::Joint& j8 = raw_skeleton.roots[1];
    j8.name = "j8";

    j0.children.resize(2);
    j0.children[0].name = "j1";
    j0.children[1].name = "j4";

    j0.children[0].children.resize(1);
    j0.children[0].children[0].name = "j2";

    j0.children[0].children[0].children.resize(1);
    j0.children[0].children[0].children[0].name = "j3";

    j0.children[1].children.resize(2);
    j0.children[1].children[0].name = "j5";
    j0.children[1].children[1].name = "j6";

    j0.children[1].children[1].children.resize(1);
    j0.children[1].children[1].children[0].name = "j7";

    j8.children.resize(1);
    j8.children[0].name = "j9";

    EXPECT_TRUE(raw_skeleton.Validate());
    EXPECT_EQ(raw_skeleton.num_joints(), 10);

    vox::unique_ptr<Skeleton> skeleton(builder(raw_skeleton));
    ASSERT_TRUE(skeleton);
    EXPECT_EQ(skeleton->num_joints(), 10);

    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 0), -12);
        EXPECT_EQ(fct.num_iterations(), 10);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 0));
        EXPECT_EQ(fct.num_iterations(), 10);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 0), Skeleton::kNoParent);
        EXPECT_EQ(fct.num_iterations(), 10);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 0), 0);
        EXPECT_EQ(fct.num_iterations(), 8);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 1), 1);
        EXPECT_EQ(fct.num_iterations(), 3);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 2), 2);
        EXPECT_EQ(fct.num_iterations(), 2);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 3), 3);
        EXPECT_EQ(fct.num_iterations(), 1);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 4), 4);
        EXPECT_EQ(fct.num_iterations(), 4);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 5), 5);
        EXPECT_EQ(fct.num_iterations(), 1);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 6), 6);
        EXPECT_EQ(fct.num_iterations(), 2);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 7), 7);
        EXPECT_EQ(fct.num_iterations(), 1);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 8), 8);
        EXPECT_EQ(fct.num_iterations(), 2);
    }
    {
        IterateDFTester fct = IterateJointsDF(*skeleton, IterateDFTester(skeleton.get(), 9), 9);
        EXPECT_EQ(fct.num_iterations(), 1);
    }
    IterateJointsDF(*skeleton, IterateDFFailTester(), 10);
    IterateJointsDF(*skeleton, IterateDFFailTester(), 99);
}

TEST(InterateDFEmpty, SkeletonUtils) {
    vox::animation::Skeleton empty;
    IterateJointsDF(empty, IterateDFFailTester(), vox::animation::Skeleton::kNoParent);
    IterateJointsDF(empty, IterateDFFailTester(), 0);
}

namespace {

class IterateDFReverseTester {
public:
    IterateDFReverseTester(const vox::animation::Skeleton* _skeleton) : skeleton_(_skeleton), num_iterations_(0) {}

    void operator()(int _current, int _parent) {
        if (num_iterations_ == 0) {
            EXPECT_TRUE(vox::animation::IsLeaf(*skeleton_, _current));
        }

        // A joint is traversed once.
        auto itc = std::find(processed_joints_.begin(), processed_joints_.end(), _current);
        EXPECT_TRUE(itc == processed_joints_.end());

        // A parent can't be traversed before a child.
        auto itp = std::find(processed_joints_.begin(), processed_joints_.end(), _parent);
        EXPECT_TRUE(itp == processed_joints_.end());

        // joint processed
        processed_joints_.push_back(_current);

        // Validates parent id.
        EXPECT_EQ(skeleton_->joint_parents()[_current], _parent);

        ++num_iterations_;
    }

    [[nodiscard]] int num_iterations() const { return num_iterations_; }

private:
    // Iterated skeleton.
    const vox::animation::Skeleton* skeleton_;

    // Number of iterations completed.
    int num_iterations_;

    // Already processed joints
    vox::vector<int> processed_joints_;
};
}  // namespace

TEST(InterateDFReverse, SkeletonUtils) {
    // Instantiates a builder objects with default parameters.
    SkeletonBuilder builder;

    RawSkeleton raw_skeleton;
    raw_skeleton.roots.resize(2);
    RawSkeleton::Joint& j0 = raw_skeleton.roots[0];
    j0.name = "j0";
    RawSkeleton::Joint& j8 = raw_skeleton.roots[1];
    j8.name = "j8";

    j0.children.resize(2);
    j0.children[0].name = "j1";
    j0.children[1].name = "j4";

    j0.children[0].children.resize(1);
    j0.children[0].children[0].name = "j2";

    j0.children[0].children[0].children.resize(1);
    j0.children[0].children[0].children[0].name = "j3";

    j0.children[1].children.resize(2);
    j0.children[1].children[0].name = "j5";
    j0.children[1].children[1].name = "j6";

    j0.children[1].children[1].children.resize(1);
    j0.children[1].children[1].children[0].name = "j7";

    j8.children.resize(1);
    j8.children[0].name = "j9";

    EXPECT_TRUE(raw_skeleton.Validate());
    EXPECT_EQ(raw_skeleton.num_joints(), 10);

    vox::unique_ptr<Skeleton> skeleton(builder(raw_skeleton));
    ASSERT_TRUE(skeleton);
    EXPECT_EQ(skeleton->num_joints(), 10);

    {
        IterateDFReverseTester fct = IterateJointsDFReverse(*skeleton, IterateDFReverseTester(skeleton.get()));
        EXPECT_EQ(fct.num_iterations(), 10);
    }
}

/* Definition of the skeleton used by the tests.
 10 joints, 2 roots

      *
    /   \
   j0    j8
 /   \     \
 j1   j4    j9
 |   / \
 j2 j5 j6
 |     |
 j3    j7
 */

TEST(IsLeaf, SkeletonUtils) {
    // Instantiates a builder objects with default parameters.
    SkeletonBuilder builder;

    RawSkeleton raw_skeleton;
    raw_skeleton.roots.resize(2);
    RawSkeleton::Joint& j0 = raw_skeleton.roots[0];
    j0.name = "j0";
    RawSkeleton::Joint& j8 = raw_skeleton.roots[1];
    j8.name = "j8";

    j0.children.resize(2);
    j0.children[0].name = "j1";
    j0.children[1].name = "j4";

    j0.children[0].children.resize(1);
    j0.children[0].children[0].name = "j2";

    j0.children[0].children[0].children.resize(1);
    j0.children[0].children[0].children[0].name = "j3";

    j0.children[1].children.resize(2);
    j0.children[1].children[0].name = "j5";
    j0.children[1].children[1].name = "j6";

    j0.children[1].children[1].children.resize(1);
    j0.children[1].children[1].children[0].name = "j7";

    j8.children.resize(1);
    j8.children[0].name = "j9";

    EXPECT_TRUE(raw_skeleton.Validate());
    EXPECT_EQ(raw_skeleton.num_joints(), 10);

    vox::unique_ptr<Skeleton> skeleton(builder(raw_skeleton));
    ASSERT_TRUE(skeleton);
    EXPECT_EQ(skeleton->num_joints(), 10);

    // Out of bound
    EXPECT_ASSERTION(IsLeaf(*skeleton, 10), "_joint index out of range");
    EXPECT_ASSERTION(IsLeaf(*skeleton, 93), "_joint index out of range");
    EXPECT_ASSERTION(IsLeaf(*skeleton, vox::animation::Skeleton::kNoParent), "_joint index out of range");

    EXPECT_FALSE(IsLeaf(*skeleton, 0));
    EXPECT_FALSE(IsLeaf(*skeleton, 1));
    EXPECT_FALSE(IsLeaf(*skeleton, 2));
    EXPECT_TRUE(IsLeaf(*skeleton, 3));
    EXPECT_FALSE(IsLeaf(*skeleton, 4));
    EXPECT_TRUE(IsLeaf(*skeleton, 5));
    EXPECT_FALSE(IsLeaf(*skeleton, 6));
    EXPECT_TRUE(IsLeaf(*skeleton, 7));
    EXPECT_FALSE(IsLeaf(*skeleton, 8));
    EXPECT_TRUE(IsLeaf(*skeleton, 9));
}

TEST(Name, SkeletonUtils) {
    // Instantiates a builder objects with default parameters.
    SkeletonBuilder builder;

    RawSkeleton raw_skeleton;
    raw_skeleton.roots.resize(4);
    raw_skeleton.roots[0].name = "j0";
    raw_skeleton.roots[1].name = "j10";
    raw_skeleton.roots[2].name = "j1";
    raw_skeleton.roots[3].name = "J0";

    vox::unique_ptr<Skeleton> skeleton(builder(raw_skeleton));
    ASSERT_TRUE(skeleton);

    EXPECT_EQ(FindJoint(*skeleton, "j0"), 0);
    EXPECT_EQ(FindJoint(*skeleton, "j10"), 1);
    EXPECT_EQ(FindJoint(*skeleton, "j1"), 2);
    EXPECT_EQ(FindJoint(*skeleton, "J0"), 3);

    EXPECT_TRUE(FindJoint(*skeleton, "aj0") < 0);
    EXPECT_TRUE(FindJoint(*skeleton, "j0a") < 0);
}