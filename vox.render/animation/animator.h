//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "vox.animation/runtime/ik_aim_job.h"
#include "vox.animation/runtime/ik_two_bone_job.h"
#include "vox.animation/runtime/local_to_model_job.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.math/bounding_box3.h"
#include "vox.render/animation/animation_state.h"
#include "vox.render/component.h"
#include "vox.simd_math/simd_quaternion.h"

namespace vox {
class Animator : public Component {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    explicit Animator(Entity* entity);

    std::shared_ptr<AnimationState> rootState();

    void setRootState(const std::shared_ptr<AnimationState>& state);

    bool loadSkeleton(const std::string& filename);

    void loadSkeleton(const vox::unique_ptr<animation::Skeleton>& skeleton);

    [[nodiscard]] const animation::Skeleton& skeleton() const;

public:
    void update(float dt);

    [[nodiscard]] bool localToModelFromExcluded() const;

    void setLocalToModelFromExcluded(bool value);

    [[nodiscard]] int localToModelFrom() const;

    void setLocalToModelFrom(int value);

    [[nodiscard]] int localToModelTo() const;

    void setLocalToModelTo(int value);

    [[nodiscard]] const vox::vector<simd_math::Float4x4>& models() const;

    // Computes the bounding box of _skeleton. This is the box that encloses all
    // skeleton's joints in model space.
    // _bound must be a valid math::Box instance.
    void computeSkeletonBounds(BoundingBox3F& bound);

public:
    void bindEntity(const std::string& name, Entity* entity);

    void scheduleTwoBoneIK(const animation::IKTwoBoneJob& data, const std::array<int, 3>& chain);

    void scheduleAimIK(const animation::IKAimJob& data, int aim);

    void scheduleLocalToModel(int from, int to = animation::Skeleton::kMaxJoints);

    struct LookAtIKData {
        // Indices of the joints that are IKed for look-at purpose.
        // Joints must be from the same hierarchy (all ancestors of the first joint
        // listed) and ordered from child to parent.
        std::vector<int> joints_chain;

        Point3F target;

        // Offset of the look at position in (head) joint local-space.
        Vector3F eyes_offset = Vector3F();
        // Overall weight given to the IK on the full chain. This allows blending in
        // and out of IK.
        float chain_weight = 1.0;
        // Weight given to every joint of the chain. If any joint has a weight of 1,
        // no other following joint will contribute (as the target will be reached).
        float joint_weight = 0.5;
    };
    void scheduleLookAtIK(const LookAtIKData& data);

    struct FloorIKData {
        // Structure used to store each leg setup data.
        struct LegSetup {
            int hip;
            int knee;
            int ankle;
        };
        std::vector<LegSetup> legs;

        // Foot height setting
        float foot_height = 0.12;
        float weight = 1.f;
        float soften = 1.f;

        Vector3F kDown{0.f, -1.f, 0.f};
        Vector3F kCharacterRayHeightOffset{0.f, 10.f, 0.f};
        Vector3F kFootRayHeightOffset{0.f, .5f, 0.f};
        bool auto_character_height = true;
        bool pelvis_correction = true;

        simd_math::SimdFloat4 kAnkleForward = -simd_math::simd_float4::x_axis();
        simd_math::SimdFloat4 kAnkleUp = simd_math::simd_float4::y_axis();
        simd_math::SimdFloat4 kKneeAxis = simd_math::simd_float4::z_axis();
    };
    void scheduleFloorIK(
            const FloorIKData& data,
            const std::function<bool(
                    const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                    raycast);

public:
    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json& data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json& data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;

private:
    void _onEnable() override;

    void _onDisable() override;

    // Multiplies a single quaternion at a specific index in a SoA transform range.
    static void _multiplySoATransformQuaternion(int _index,
                                                const simd_math::SimdQuaternion& _quat,
                                                const span<simd_math::SoaTransform>& _transforms);

    // Computes the bounding box of posture defines be _matrices range.
    // _bound must be a valid math::Box instance.
    static void _computePostureBounds(span<const simd_math::Float4x4> _matrices, BoundingBox3F* _bound);

    // Raycast down from the current position to find character height on the
    // floor. It directly updates root translation as output.
    void _updateCharacterHeight(
            const FloorIKData& data,
            const std::function<bool(
                    const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                    raycast);

    // For each leg, raycasts a vector going down from the ankle position.
    // This allows to find the intersection point with the floor.
    void _raycastLegs(
            const FloorIKData& data,
            const std::function<bool(
                    const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                    raycast);

    // Computes ankle target position (C), so that the foot is in contact with
    // the floor. Because of floor slope (defined by raycast intersection normal),
    // ankle position cannot be simply be offseted by foot offset. See geogebra
    // diagram for more details: media/doc/samples/foot_ik_ankle.ggb
    void _updateAnklesTarget(const FloorIKData& data);

    // Recomputes pelvis offset.
    // Strategy is to move the pelvis along "down" axis (ray axis), enough for
    // the lowest foot (lowest from its original position) to reaches ankle
    // target. The other foot will be ik-ed.
    void _updatePelvisOffset(const FloorIKData& data);

    // Applies two bone IK to the leg, and aim IK to the ankle
    bool _updateFootIK(const FloorIKData& data);

    // This function will compute two bone IK on the leg, updating hip and knee
    // rotations so that ankle can reach its targetted position.
    bool _applyLegTwoBoneIK(const FloorIKData& data,
                            const FloorIKData::LegSetup& _leg,
                            const Vector3F& _target_ws,
                            const simd_math::Float4x4& _inv_root);

    // This function will compute aim IK on the ankle, updating its rotations so
    // it can be aligned with the floor.
    // The strategy is to align ankle up vector in the direction of the floor
    // normal. The forward direction of the foot is then driven by the pole
    // vector, which polls the foot (ankle forward vector) toward it's original
    // (animated) direction.
    bool _applyAnkleAimIK(const FloorIKData& data,
                          const FloorIKData::LegSetup& _leg,
                          const Vector3F& _target_ws,
                          const simd_math::Float4x4& _inv_root);

private:
    animation::Skeleton _skeleton;
    animation::LocalToModelJob _ltm_job;
    // Buffer of local transforms as sampled from animation_.
    vox::vector<simd_math::SoaTransform> _locals;
    // Buffer of model space matrices.
    vox::vector<simd_math::Float4x4> _models;
    std::shared_ptr<AnimationState> _rootState{nullptr};
    std::vector<std::function<void()>> _scheduleFunctor{};
    std::unordered_map<size_t, std::unordered_set<Entity*>> _entityBindingMap{};

    struct LegRayInfo {
        Vector3F start;
        Vector3F dir;

        bool hit{};
        Vector3F hit_point;
        Vector3F hit_normal;
    };
    std::vector<LegRayInfo> _rays_info;
    std::vector<Vector3F> _ankles_initial_ws;
    std::vector<Vector3F> _ankles_target_ws;
    Vector3F pelvis_offset{};
};
}  // namespace vox
