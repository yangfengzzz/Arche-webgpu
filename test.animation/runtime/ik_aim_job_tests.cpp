//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.animation/gtest_math_helper.h"
#include "vox.animation/runtime/ik_aim_job.h"
#include "vox.math/quaternion.h"
#include "vox.simd_math/simd_quaternion.h"

TEST(JobValidity, IKAimJob) {
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    vox::simd_math::SimdQuaternion quat{};

    {  // Default is invalid
        vox::animation::IKAimJob job;
        EXPECT_FALSE(job.Validate());
    }

    {  // Invalid joint matrix
        vox::animation::IKAimJob job;
        job.joint = &joint;
        EXPECT_FALSE(job.Validate());
    }

    {  // Invalid output
        vox::animation::IKAimJob job;
        job.joint_correction = &quat;
        EXPECT_FALSE(job.Validate());
    }

    {  // Invalid non normalized forward vector.
        vox::animation::IKAimJob job;
        job.forward = vox::simd_math::simd_float4::Load(.5f, 0.f, 0.f, 0.f);
        EXPECT_FALSE(job.Validate());
    }

    {  // Valid
        vox::animation::IKAimJob job;
        job.joint = &joint;
        job.joint_correction = &quat;
        EXPECT_TRUE(job.Validate());
        EXPECT_TRUE(job.Run());
    }
}

TEST(Correction, IKAimJob) {
    vox::simd_math::SimdQuaternion quat{};

    vox::animation::IKAimJob job;
    job.joint_correction = &quat;

    // Test will be executed with different root transformations
    const vox::simd_math::Float4x4 parents[] = {
            vox::simd_math::Float4x4::identity(),                                          // No root transformation
            vox::simd_math::Float4x4::Translation(vox::simd_math::simd_float4::y_axis()),  // Up
            vox::simd_math::Float4x4::FromEuler(
                    vox::simd_math::simd_float4::Load(vox::kPiF / 3.f, 0.f, 0.f, 0.f)),  // Rotated
            vox::simd_math::Float4x4::Scaling(
                    vox::simd_math::simd_float4::Load(2.f, 2.f, 2.f, 0.f)),  // Uniformly scaled
            vox::simd_math::Float4x4::Scaling(
                    vox::simd_math::simd_float4::Load(1.f, 2.f, 1.f, 0.f)),  // Non-uniformly scaled
            vox::simd_math::Float4x4::Scaling(vox::simd_math::simd_float4::Load(-3.f, -3.f, -3.f, 0.f))  // Mirrored
    };

    for (const auto& parent : parents) {
        job.joint = &parent;

        // These are in joint local-space
        job.forward = vox::simd_math::simd_float4::x_axis();
        job.up = vox::simd_math::simd_float4::y_axis();

        // Pole vector is in model space
        job.pole_vector = TransformVector(parent, vox::simd_math::simd_float4::y_axis());

        {  // x
            job.target = TransformPoint(parent, vox::simd_math::simd_float4::x_axis());
            EXPECT_TRUE(job.Run());
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
        }

        {  // -x
            job.target = TransformPoint(parent, -vox::simd_math::simd_float4::x_axis());
            EXPECT_TRUE(job.Run());
            const vox::QuaternionF y_Pi = vox::QuaternionF(vox::Vector3F::y_axis(), vox::kPiF);
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_Pi.x, y_Pi.y, y_Pi.z, y_Pi.w, 2e-3f);
        }

        {  // z
            job.target = TransformPoint(parent, vox::simd_math::simd_float4::z_axis());
            EXPECT_TRUE(job.Run());
            const vox::QuaternionF y_mPi_2 = vox::QuaternionF(vox::Vector3F::y_axis(), -vox::kHalfPiF);
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_mPi_2.x, y_mPi_2.y, y_mPi_2.z, y_mPi_2.w, 2e-3f);
        }

        {  // -z
            job.target = TransformPoint(parent, -vox::simd_math::simd_float4::z_axis());
            EXPECT_TRUE(job.Run());
            const vox::QuaternionF y_Pi_2 = vox::QuaternionF(vox::Vector3F::y_axis(), vox::kHalfPiF);
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_Pi_2.x, y_Pi_2.y, y_Pi_2.z, y_Pi_2.w, 2e-3f);
        }

        {  // 45 up y
            job.target = TransformPoint(parent, vox::simd_math::simd_float4::Load(1.f, 1.f, 0.f, 0.f));
            EXPECT_TRUE(job.Run());
            const vox::QuaternionF z_Pi_4 = vox::QuaternionF(vox::Vector3F::z_axis(), vox::kQuarterPiF);
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_4.x, z_Pi_4.y, z_Pi_4.z, z_Pi_4.w, 2e-3f);
        }

        {  // 45 up y, further
            job.target = TransformPoint(parent, vox::simd_math::simd_float4::Load(2.f, 2.f, 0.f, 0.f));
            EXPECT_TRUE(job.Run());
            const vox::QuaternionF z_Pi_4 = vox::QuaternionF(vox::Vector3F::z_axis(), vox::kQuarterPiF);
            EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_4.x, z_Pi_4.y, z_Pi_4.z, z_Pi_4.w, 2e-3f);
        }
    }
}

TEST(Forward, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    {  // forward x
        job.forward = vox::simd_math::simd_float4::x_axis();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // forward -x
        job.forward = -vox::simd_math::simd_float4::x_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_Pi = vox::QuaternionF(vox::Vector3F::y_axis(), -vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_Pi.x, y_Pi.y, y_Pi.z, y_Pi.w, 2e-3f);
    }

    {  // forward z
        job.forward = vox::simd_math::simd_float4::z_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_Pi_2 = vox::QuaternionF(vox::Vector3F::y_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_Pi_2.x, y_Pi_2.y, y_Pi_2.z, y_Pi_2.w, 2e-3f);
    }
}

TEST(Up, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::x_axis();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    {  // up y
        job.up = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // up -y
        job.up = -vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi.x, x_Pi.y, x_Pi.z, x_Pi.w, 2e-3f);
    }

    {  // up z
        job.up = vox::simd_math::simd_float4::z_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_mPi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_mPi_2.x, x_mPi_2.y, x_mPi_2.z, x_mPi_2.w, 2e-3f);
    }

    {  // up 2*z
        job.up = vox::simd_math::simd_float4::z_axis() * vox::simd_math::simd_float4::Load1(2.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_mPi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_mPi_2.x, x_mPi_2.y, x_mPi_2.z, x_mPi_2.w, 2e-3f);
    }

    {  // up very small z
        job.up = vox::simd_math::simd_float4::z_axis() * vox::simd_math::simd_float4::Load1(1e-9f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_mPi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_mPi_2.x, x_mPi_2.y, x_mPi_2.z, x_mPi_2.w, 2e-3f);
    }

    {  // up is zero
        job.up = vox::simd_math::simd_float4::zero();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }
}

TEST(Pole, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::x_axis();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();

    {  // Pole y
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // Pole -y
        job.pole_vector = -vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi.x, x_Pi.y, x_Pi.z, x_Pi.w, 2e-3f);
    }

    {  // Pole z
        job.pole_vector = vox::simd_math::simd_float4::z_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi_2.x, x_Pi_2.y, x_Pi_2.z, x_Pi_2.w, 2e-3f);
    }

    {  // Pole 2*z
        job.pole_vector = vox::simd_math::simd_float4::z_axis() * vox::simd_math::simd_float4::Load1(2.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi_2.x, x_Pi_2.y, x_Pi_2.z, x_Pi_2.w, 2e-3f);
    }

    {  // Pole very small z
        job.pole_vector = vox::simd_math::simd_float4::z_axis() * vox::simd_math::simd_float4::Load1(1e-9f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi_2.x, x_Pi_2.y, x_Pi_2.z, x_Pi_2.w, 2e-3f);
    }
}

TEST(Offset, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;
    bool reached;
    job.reached = &reached;

    job.target = vox::simd_math::simd_float4::x_axis();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    {  // No offset
        reached = false;
        job.offset = vox::simd_math::simd_float4::zero();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset inside target sphere
        reached = false;
        job.offset = vox::simd_math::simd_float4::Load(0.f, vox::kSqrt2_2, 0.f, 0.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_4 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kQuarterPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_4.x, z_Pi_4.y, z_Pi_4.z, z_Pi_4.w, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset inside target sphere
        reached = false;
        job.offset = vox::simd_math::simd_float4::Load(.5f, .5f, 0.f, 0.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_6 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kPiF / 6.f);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_6.x, z_Pi_6.y, z_Pi_6.z, z_Pi_6.w, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset inside target sphere
        reached = false;
        job.offset = vox::simd_math::simd_float4::Load(-.5f, .5f, 0.f, 0.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_6 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kPiF / 6.f);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_6.x, z_Pi_6.y, z_Pi_6.z, z_Pi_6.w, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset inside target sphere
        reached = false;
        job.offset = vox::simd_math::simd_float4::Load(.5f, 0.f, .5f, 0.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_Pi_6 = vox::QuaternionF(vox::Vector3F::y_axis(), vox::kPiF / 6.f);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_Pi_6.x, y_Pi_6.y, y_Pi_6.z, y_Pi_6.w, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset on target sphere
        reached = false;
        job.offset = vox::simd_math::simd_float4::Load(0.f, 1.f, 0.f, 0.f);
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_2.x, z_Pi_2.y, z_Pi_2.z, z_Pi_2.w, 2e-3f);
        EXPECT_TRUE(reached);
    }

    {  // Offset outside of target sphere, unreachable
        reached = true;
        job.offset = vox::simd_math::simd_float4::Load(0.f, 2.f, 0.f, 0.f);
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
        EXPECT_FALSE(reached);
    }

    const vox::simd_math::Float4x4 translated_joint =
            vox::simd_math::Float4x4::Translation(vox::simd_math::simd_float4::y_axis());
    job.joint = &translated_joint;

    {  // Offset inside of target sphere, unreachable
        reached = false;
        job.offset = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_2.x, z_Pi_2.y, z_Pi_2.z, z_Pi_2.w, 2e-3f);
        EXPECT_TRUE(reached);
    }
}

TEST(Twist, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::x_axis();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();

    {  // Pole y, twist 0
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        job.twist_angle = 0.f;
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // Pole y, twist pi
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        job.twist_angle = vox::kPiF;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi = vox::QuaternionF(vox::Vector3F::x_axis(), -vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi.x, x_Pi.y, x_Pi.z, x_Pi.w, 2e-3f);
    }

    {  // Pole y, twist -pi
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        job.twist_angle = -vox::kPiF;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_mPi = vox::QuaternionF(vox::Vector3F::x_axis(), -vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_mPi.x, x_mPi.y, x_mPi.z, x_mPi.w, 2e-3f);
    }

    {  // Pole y, twist pi/2
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        job.twist_angle = vox::kHalfPiF;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi_2 = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi_2.x, x_Pi_2.y, x_Pi_2.z, x_Pi_2.w, 2e-3f);
    }

    {  // Pole z, twist pi/2
        job.pole_vector = vox::simd_math::simd_float4::z_axis();
        job.twist_angle = vox::kHalfPiF;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF x_Pi = vox::QuaternionF(vox::Vector3F::x_axis(), vox::kPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, x_Pi.x, x_Pi.y, x_Pi.z, x_Pi.w, 2e-3f);
    }
}

TEST(AlignedTargetUp, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.forward = vox::simd_math::simd_float4::x_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    {  // Not aligned
        job.target = vox::simd_math::simd_float4::x_axis();
        job.up = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // Aligned y
        job.target = vox::simd_math::simd_float4::y_axis();
        job.up = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_2.x, z_Pi_2.y, z_Pi_2.z, z_Pi_2.w, 2e-3f);
    }

    {  // Aligned 2*y
        job.target = vox::simd_math::simd_float4::y_axis() * vox::simd_math::simd_float4::Load1(2.f);
        job.up = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_2.x, z_Pi_2.y, z_Pi_2.z, z_Pi_2.w, 2e-3f);
    }

    {  // Aligned -2*y
        job.target = vox::simd_math::simd_float4::y_axis() * vox::simd_math::simd_float4::Load1(-2.f);
        job.up = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_mPi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_mPi_2.x, z_mPi_2.y, z_mPi_2.z, z_mPi_2.w, 2e-3f);
    }
}

TEST(AlignedTargetPole, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();

    {  // Not aligned
        job.target = vox::simd_math::simd_float4::x_axis();
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // Aligned y
        job.target = vox::simd_math::simd_float4::y_axis();
        job.pole_vector = vox::simd_math::simd_float4::y_axis();
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF z_Pi_2 = vox::QuaternionF(vox::Vector3F::z_axis(), vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, z_Pi_2.x, z_Pi_2.y, z_Pi_2.z, z_Pi_2.w, 2e-3f);
    }
}

TEST(TargetTooClose, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::zero();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    EXPECT_TRUE(job.Run());
    EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
}

TEST(Weight, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::identity();
    job.joint = &joint;

    job.target = vox::simd_math::simd_float4::z_axis();
    job.forward = vox::simd_math::simd_float4::x_axis();
    job.up = vox::simd_math::simd_float4::y_axis();
    job.pole_vector = vox::simd_math::simd_float4::y_axis();

    {  // Full weight
        job.weight = 1.f;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_mPi2 = vox::QuaternionF(vox::Vector3F::y_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_mPi2.x, y_mPi2.y, y_mPi2.z, y_mPi2.w, 2e-3f);
    }

    {  // > 1
        job.weight = 2.f;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_mPi2 = vox::QuaternionF(vox::Vector3F::y_axis(), -vox::kHalfPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_mPi2.x, y_mPi2.y, y_mPi2.z, y_mPi2.w, 2e-3f);
    }

    {  // Half weight
        job.weight = .5f;
        EXPECT_TRUE(job.Run());
        const vox::QuaternionF y_mPi4 = vox::QuaternionF(vox::Vector3F::y_axis(), -vox::kQuarterPiF);
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, y_mPi4.x, y_mPi4.y, y_mPi4.z, y_mPi4.w, 2e-3f);
    }

    {  // Zero weight
        job.weight = 0.f;
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }

    {  // < 0
        job.weight = -.5f;
        EXPECT_TRUE(job.Run());
        EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
    }
}

TEST(ZeroScale, IKAimJob) {
    vox::animation::IKAimJob job;
    vox::simd_math::SimdQuaternion quat{};
    job.joint_correction = &quat;
    const vox::simd_math::Float4x4 joint = vox::simd_math::Float4x4::Scaling(vox::simd_math::simd_float4::zero());
    job.joint = &joint;

    EXPECT_TRUE(job.Run());
    EXPECT_SIMDQUATERNION_EQ_TOL(quat, 0.f, 0.f, 0.f, 1.f, 2e-3f);
}
