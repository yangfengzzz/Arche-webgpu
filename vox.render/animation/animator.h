//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

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

    void update(float dt);

    [[nodiscard]] bool localToModelFromExcluded() const;

    void setLocalToModelFromExcluded(bool value);

    [[nodiscard]] int localToModelFrom() const;

    void setLocalToModelFrom(int value);

    [[nodiscard]] int localToModelTo() const;

    void setLocalToModelTo(int value);

    [[nodiscard]] const vox::vector<simd_math::Float4x4>& models() const;

    animation::Skeleton& skeleton();

    // Computes the bounding box of _skeleton. This is the box that encloses all
    // skeleton's joints in model space.
    // _bound must be a valid math::Box instance.
    void computeSkeletonBounds(BoundingBox3F& bound);

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

private:
    animation::Skeleton _skeleton;
    animation::LocalToModelJob _ltm_job;
    // Buffer of model space matrices.
    vox::vector<simd_math::Float4x4> _models;
    std::shared_ptr<AnimationState> _rootState{nullptr};
};
}  // namespace vox
