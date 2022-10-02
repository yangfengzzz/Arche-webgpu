//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "vox.animation/runtime/local_to_model_job.h"
#include "vox.base/memory/unique_ptr.h"
#include "vox.math/bounding_box3.h"
#include "vox.render/animation/animation_state.h"
#include "vox.render/animation/avatar.h"
#include "vox.render/component.h"
#include "vox.simd_math/simd_quaternion.h"

namespace vox {
class Animator : public Component {
public:
    Avatar avatar;

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

    void scheduleTwoBoneIK(int start_joint,
                           int mid_joint,
                           int end_joint,
                           const Vector3F& target_ws,
                           float soften = 1.f,
                           float weight = 1.f,
                           float twist_angle = 0.f,
                           std::optional<Vector3F> pole_vector = std::nullopt,
                           const Vector3F& pelvis_offset = Vector3F());

    void scheduleAimIK(const int& target_joint,
                       const Vector3F& target_ws,
                       float weight = 1.f,
                       const Vector3F& pelvis_offset = Vector3F());

    void scheduleLocalToModel(int from, int to = animation::Skeleton::kMaxJoints);

    void clearAllSchedule();

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

    [[nodiscard]] simd_math::Float4x4 _getRootTransform(const Vector3F& pelvis_offset) const;

    // Multiplies a single quaternion at a specific index in a SoA transform range.
    static void _multiplySoATransformQuaternion(int _index,
                                                const simd_math::SimdQuaternion& _quat,
                                                const span<simd_math::SoaTransform>& _transforms);

    // Computes the bounding box of posture defines be _matrices range.
    // _bound must be a valid math::Box instance.
    static void _computePostureBounds(span<const simd_math::Float4x4> _matrices, BoundingBox3F* _bound);

private:
    enum ScheduleType { TargetIK, TwoBoneIK, LocalToModel };
    struct ScheduleData {
        void* ptr{nullptr};
        ScheduleType type{};
    };
    std::vector<ScheduleData> _scheduleData{};

    struct TwoBoneIKData {
        int start_joint;
        int mid_joint;
        int end_joint;
        float soften;
        float twist_angle;
        Vector3F target_ws;
        float weight;
        Vector3F pelvis_offset;
        std::optional<Vector3F> pole_vector;
    };
    std::vector<TwoBoneIKData> _twoBoneIKData{};
    void _applyTwoBoneIK(const TwoBoneIKData& data);

    struct AimIKData {
        int target_joint;
        Vector3F target_ws;
        float weight;
        Vector3F pelvis_offset;
    };
    std::vector<AimIKData> _targetIKData{};

    void _applyAimIK(const AimIKData& data);

    struct LocalToModelData {
        int from;
        int to;
    };
    std::vector<LocalToModelData> _ltmData{};

    void _applyLocalToModel(const LocalToModelData& data);

private:
    animation::Skeleton _skeleton;
    animation::LocalToModelJob _ltm_job;
    // Buffer of local transforms as sampled from animation_.
    vox::vector<simd_math::SoaTransform> _locals;
    // Buffer of model space matrices.
    vox::vector<simd_math::Float4x4> _models;
    std::shared_ptr<AnimationState> _rootState{nullptr};

    std::unordered_map<size_t, std::unordered_set<Entity*>> _entityBindingMap{};
};
}  // namespace vox
