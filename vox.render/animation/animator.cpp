//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

#include <utility>

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/platform/filesystem.h"

namespace vox {
std::string Animator::name() { return "Animator"; }

Animator::Animator(Entity* entity) : Component(entity) {}

std::shared_ptr<AnimationState> Animator::rootState() { return _rootState; }

void Animator::setRootState(const std::shared_ptr<AnimationState>& state) { _rootState = state; }

bool Animator::loadSkeleton(const std::string& filename) {
    LOGI("Loading skeleton archive {}", filename)
    vox::io::File file((fs::path::Get(fs::path::Type::ASSETS) + filename).c_str(), "rb");
    if (!file.opened()) {
        LOGE("Failed to open skeleton file {}", filename)
        return false;
    }
    vox::io::IArchive archive(&file);
    if (!archive.TestTag<vox::animation::Skeleton>()) {
        LOGE("Failed to load skeleton instance from file {}.", filename)
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> _skeleton;

    _models.resize(_skeleton.num_joints());
    _ltm_job.output = make_span(_models);
    _ltm_job.skeleton = &_skeleton;
    return true;
}

void Animator::loadSkeleton(const vox::unique_ptr<animation::Skeleton>& skeleton) {
    _skeleton = std::move(*skeleton);
    _models.resize(_skeleton.num_joints());
    _ltm_job.output = make_span(_models);
    _ltm_job.skeleton = &_skeleton;
}

void Animator::update(float dt) {
    if (_rootState) {
        _rootState->loadSkeleton(&_skeleton);
        _rootState->update(dt);
        _locals = _rootState->locals();
    } else {
        _locals.resize(_skeleton.num_soa_joints());
        // Initialize locals from skeleton rest pose
        for (size_t i = 0; i < _locals.size(); ++i) {
            _locals[i] = _skeleton.joint_rest_poses()[i];
        }
    }
    _ltm_job.input = make_span(_locals);
    (void)_ltm_job.Run();

    // post-sample schedule work
    for (const auto& functor : _scheduleFunctor) {
        functor();
    }
    _scheduleFunctor.clear();

    // sync to attach entity
    Matrix4x4F localMatrix;
    for (const auto& entities : _entityBindingMap) {
        size_t index = entities.first;
        memcpy(localMatrix.data(), &_models[index].cols[0], sizeof(Matrix4x4F));
        for (auto& entity : entities.second) {
            entity->transform->setLocalMatrix(localMatrix);
        }
    }
}

bool Animator::localToModelFromExcluded() const { return _ltm_job.from_excluded; }

void Animator::setLocalToModelFromExcluded(bool value) { _ltm_job.from_excluded = value; }

int Animator::localToModelFrom() const { return _ltm_job.from; }

void Animator::setLocalToModelFrom(int value) { _ltm_job.from = value; }

int Animator::localToModelTo() const { return _ltm_job.to; }

void Animator::setLocalToModelTo(int value) { _ltm_job.to = value; }

const vox::vector<simd_math::Float4x4>& Animator::models() const { return _models; }

const animation::Skeleton& Animator::skeleton() const { return _skeleton; }

void Animator::_multiplySoATransformQuaternion(int index,
                                               const simd_math::SimdQuaternion& quat,
                                               const span<simd_math::SoaTransform>& transforms) {
    assert(index >= 0 && static_cast<size_t>(index) < transforms.size() * 4 && "joint index out of bound.");

    // Convert soa to aos in order to perform quaternion multiplication, and gets
    // back to soa.
    vox::simd_math::SoaTransform& soa_transform_ref = transforms[index / 4];
    vox::simd_math::SimdQuaternion aos_quaternions[4];
    vox::simd_math::Transpose4x4(&soa_transform_ref.rotation.x, &aos_quaternions->xyzw);

    vox::simd_math::SimdQuaternion& aos_quat_ref = aos_quaternions[index & 3];
    aos_quat_ref = aos_quat_ref * quat;

    vox::simd_math::Transpose4x4(&aos_quaternions->xyzw, &soa_transform_ref.rotation.x);
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

// MARK: - Reflection
void Animator::onSerialize(nlohmann::json& data) {}

void Animator::onDeserialize(nlohmann::json& data) {}

void Animator::onInspector(ui::WidgetContainer& p_root) {}

void Animator::bindEntity(const std::string& name, Entity* entity) {
    auto iter = std::find(_skeleton.joint_names().begin(), _skeleton.joint_names().end(), name);
    if (iter != _skeleton.joint_names().end()) {
        _entityBindingMap[iter - _skeleton.joint_names().begin()].emplace(entity);
    }
}

void Animator::encodeHandIKData(const HandIKData& data) {
    _scheduleFunctor.emplace_back([this, data]() {
        // Target and pole should be in model-space, so they must be converted from
        // world-space using character inverse root matrix.
        // IK jobs must support non-invertible matrices (like 0 scale matrices).
        auto worldMat = entity()->transform->worldMatrix();
        simd_math::Float4x4 root{};
        memcpy(&root.cols[0], worldMat.data(), 64);
        simd_math::SimdInt4 invertible;
        const simd_math::Float4x4 invert_root = Invert(root, &invertible);

        const simd_math::SimdFloat4 target_ms =
                TransformPoint(invert_root, simd_math::simd_float4::Load3PtrU(&data.target.x));
        const simd_math::SimdFloat4 pole_vector_ms =
                TransformVector(invert_root, simd_math::simd_float4::Load3PtrU(&data.pole_vector.x));

        // Setup IK job.
        animation::IKTwoBoneJob ik_job;
        ik_job.target = target_ms;
        ik_job.pole_vector = pole_vector_ms;
        ik_job.mid_axis = simd_math::simd_float4::z_axis();  // Middle joint
                                                             // rotation axis is
                                                             // fixed, and depends
                                                             // on skeleton rig.
        ik_job.weight = data.weight;
        ik_job.soften = data.soften;
        ik_job.twist_angle = data.twist_angle;

        // Provides start, middle and end joints model space matrices.
        ik_job.start_joint = &_models[data.start_joint];
        ik_job.mid_joint = &_models[data.mid_joint];
        ik_job.end_joint = &_models[data.end_joint];

        // Setup output pointers.
        simd_math::SimdQuaternion start_correction{};
        ik_job.start_joint_correction = &start_correction;
        simd_math::SimdQuaternion mid_correction{};
        ik_job.mid_joint_correction = &mid_correction;

        if (!ik_job.Run()) {
            return;
        }

        // Apply IK quaternions to their respective local-space transforms.
        _multiplySoATransformQuaternion(data.start_joint, start_correction, make_span(_locals));
        _multiplySoATransformQuaternion(data.mid_joint, mid_correction, make_span(_locals));

        // Updates model-space matrices now IK has been applied to local transforms.
        // All the ancestors of the start of the IK chain must be computed.
        animation::LocalToModelJob ltm_job;
        ltm_job.skeleton = &_skeleton;
        ltm_job.input = make_span(_locals);
        ltm_job.output = make_span(_models);
        ltm_job.from = data.start_joint;  // Local transforms haven't changed before start_joint_.
        ltm_job.to = animation::Skeleton::kMaxJoints;

        if (!ltm_job.Run()) {
            return;
        }
    });
}

void Animator::encodeLookAtIK(const LookAtIKData& data) {
    _scheduleFunctor.emplace_back([this, data]() {
        // IK aim job setup.
        animation::IKAimJob ik_job;

        // Pole vector and target position are constant for the whole algorithm, in
        // model-space.
        ik_job.pole_vector = simd_math::simd_float4::y_axis();
        ik_job.target = simd_math::simd_float4::Load3PtrU(&data.target.x);

        // The same quaternion will be used each time the job is run.
        simd_math::SimdQuaternion correction{};
        ik_job.joint_correction = &correction;

        // The algorithm iteratively updates from the first joint (closer to the
        // head) to the last (the further ancestor, closer to the pelvis). Joints
        // order is already validated. For the first joint, aim IK is applied with
        // the global forward and offset, so the forward vector aligns in direction
        // of the target. If a weight lower than 1 is provided to the first joint,
        // then it will not fully align to the target. In this case further joint
        // will need to be updated. For the remaining joints, forward vector and
        // offset position are computed in each joint local-space, before IK is
        // applied:
        // 1. Rotates forward and offset position based on the result of the
        // previous joint IK.
        // 2. Brings forward and offset back in joint local-space.
        // Aim is iteratively applied up to the last selected joint of the
        // hierarchy. A weight of 1 is given to the last joint, so we can guarantee
        // target is reached. Note that model-space transform of each joint doesn't
        // need to be updated between each pass, as joints are ordered from child to
        // parent.
        int previous_joint = animation::Skeleton::kNoParent;
        for (int i = 0, joint = data.joints_chain[0]; i < data.joints_chain.size();
             ++i, previous_joint = joint, joint = data.joints_chain[i]) {
            // Setups the model-space matrix of the joint being processed by IK.
            ik_job.joint = &_models[joint];

            // Setups joint local-space up vector.
            ik_job.up = simd_math::simd_float4::x_axis();

            // Setups weights of IK job.
            // the last joint being processed needs a full weight (1.f) to ensure
            // target is reached.
            const bool last = i == data.joints_chain.size() - 1;
            ik_job.weight = data.chain_weight * (last ? 1.f : data.joint_weight);

            // Setup offset and forward vector for the current joint being processed.
            if (i == 0) {
                // First joint, uses global forward and offset.
                ik_job.offset = simd_math::simd_float4::Load3PtrU(&data.eyes_offset.x);
                ik_job.forward = simd_math::simd_float4::y_axis();
            } else {
                // Applies previous correction to "forward" and "offset", before
                // bringing them to model-space (_ms).
                const simd_math::SimdFloat4 corrected_forward_ms =
                        TransformVector(_models[previous_joint], TransformVector(correction, ik_job.forward));
                const simd_math::SimdFloat4 corrected_offset_ms =
                        TransformPoint(_models[previous_joint], TransformVector(correction, ik_job.offset));

                // Brings "forward" and "offset" to joint local-space
                const simd_math::Float4x4 inv_joint = Invert(_models[joint]);
                ik_job.forward = TransformVector(inv_joint, corrected_forward_ms);
                ik_job.offset = TransformPoint(inv_joint, corrected_offset_ms);
            }

            // Runs IK aim job.
            if (!ik_job.Run()) {
                return;
            }

            // Apply IK quaternion to its respective local-space transforms.
            _multiplySoATransformQuaternion(joint, correction, make_span(_locals));
        }

        // Skeleton model-space matrices need to be updated again. This re-uses the
        // already setup job, but limits the update to childs of the last joint (the
        // parent-iest of the chain).
        _ltm_job.from = previous_joint;
        (void)_ltm_job.Run();
    });
}

void Animator::encodeFloorIK(
        const FloorIKData& data,
        const std::function<
                bool(const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                raycast) {
    _scheduleFunctor.emplace_back([this, data, raycast]() {
        _rays_info.resize(data.legs.size());
        _ankles_initial_ws.resize(data.legs.size());
        _ankles_target_ws.resize(data.legs.size());

        // Finds character height on the floor, evaluated at its root position.
        _updateCharacterHeight(data, raycast);

        // For each leg, raycasts a vector going down from the ankle position.
        // This allows to find the intersection point with the floor.
        _raycastLegs(data, raycast);

        // Computes targeted ankles positions, taking floor steepness and foot
        // height in consideration.
        _updateAnklesTarget(data);

        // Offsets the character down, so that the lowest ankle (lowest from its
        // original position) reaches its targeted position. The other leg(s) will
        // be ik-ed.
        _updatePelvisOffset(data);

        // Updates legs and ankles transforms, so they reach their targeted
        // position and orientation.
        _updateFootIK(data);
    });
}

void Animator::_updateCharacterHeight(
        const FloorIKData& data,
        const std::function<
                bool(const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                raycast) {
    if (!data.auto_character_height) {
        return;
    }

    // Starts the ray from above (kCharacterRayHeightOffset) current character
    // position.
    auto worldPos = entity()->transform->worldPosition();
    auto root_translation = Vector3F(worldPos.x, worldPos.y, worldPos.z);
    raycast(root_translation + data.kCharacterRayHeightOffset, data.kDown, &root_translation, nullptr);
}

void Animator::_raycastLegs(
        const FloorIKData& data,
        const std::function<
                bool(const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect, Vector3F* normal)>&
                raycast) {
    // Pelvis offset isn't updated yet, it shouldn't be considered. So we're
    // using "unoffsetted" root transform.
    auto worldMat = entity()->transform->worldMatrix();
    simd_math::Float4x4 root{};
    memcpy(&root.cols[0], worldMat.data(), 64);

    // Raycast down for each leg to find the intersection point with the floor.
    for (size_t l = 0; l < data.legs.size(); ++l) {
        const FloorIKData::LegSetup& leg = data.legs[l];
        LegRayInfo& ray = _rays_info[l];

        // Finds ankle initial world space position
        simd_math::Store3PtrU(TransformPoint(root, _models[leg.ankle].cols[3]), &_ankles_initial_ws[l].x);

        // Builds ray, from above ankle (kFootRayHeightOffset) and going downward.
        ray.start = _ankles_initial_ws[l] + data.kFootRayHeightOffset;
        ray.dir = data.kDown;
        ray.hit = raycast(ray.start, ray.dir, &ray.hit_point, &ray.hit_normal);
    }
}

void Animator::_updateAnklesTarget(const FloorIKData& data) {
    for (size_t l = 0; l < data.legs.size(); ++l) {
        const LegRayInfo& ray = _rays_info[l];
        if (!ray.hit) {
            continue;
        }

        // Computes projection of the ray AI (from start to floor intersection
        // point) onto floor normal. This gives the length of segment AB.
        // Note that ray.hit_normal is normalized already.
        const float ABl = (ray.start - ray.hit_point).dot(ray.hit_normal);
        if (ABl == 0.f) {
            // Early out if the two are perpendicular.
            continue;
        }

        // Knowing A, AB length and direction, we can compute B position.
        const Vector3F B = ray.start - ray.hit_normal * ABl;

        // Computes segment IB and its length (IBl)
        const Vector3F IB = B - ray.hit_point;
        const float IBl = IB.length();

        if (IBl <= 0.f) {
            // If B is at raycast intersection (I), then we still need to update
            // corrected ankle position (world-space) to take into account foot
            // height.
            _ankles_target_ws[l] = ray.hit_point + ray.hit_normal * data.foot_height;
        } else {
            // HC length is known (as foot height). So we're using Thales theorem to
            // find H position.
            const float IHl = IBl * data.foot_height / ABl;
            const Vector3F IH = IB * (IHl / IBl);
            const Vector3F H = ray.hit_point + IH;

            // C (Corrected ankle) position can now be found.
            const Vector3F C = H + ray.hit_normal * data.foot_height;

            // Override ankle position with result.
            _ankles_target_ws[l] = C;
        }
    }
}

void Animator::_updatePelvisOffset(const FloorIKData& data) {
    pelvis_offset = Vector3F(0.f, 0.f, 0.f);

    float max_dot = -std::numeric_limits<float>::max();
    if (data.pelvis_correction) {
        for (size_t l = 0; l < data.legs.size(); ++l) {
            const LegRayInfo& ray = _rays_info[l];
            if (!ray.hit) {
                continue;
            }

            // Check if this ankle is lower (in down direction) compared to the
            // previous one.
            const Vector3F ankle_to_target = _ankles_target_ws[l] - _ankles_initial_ws[l];
            const float dot = ankle_to_target.dot(data.kDown);
            if (dot > max_dot) {
                max_dot = dot;

                // Compute offset using the maximum displacement that the legs should
                // have to touch ground.
                pelvis_offset = data.kDown * dot;
            }
        }
    }
}

bool Animator::_updateFootIK(const FloorIKData& data) {
    // Pelvis offset needs to be considered when converting to model space. So
    // we're using "offset" root transform.
    auto worldMat = entity()->transform->worldMatrix();
    if (data.pelvis_correction) {
        worldMat(3, 0) += pelvis_offset.x;
        worldMat(3, 1) += pelvis_offset.y;
        worldMat(3, 2) += pelvis_offset.z;
    }
    simd_math::Float4x4 root{};
    memcpy(&root.cols[0], worldMat.data(), 64);
    const simd_math::Float4x4 inv_root = Invert(root);

    // Perform IK
    for (size_t l = 0; l < data.legs.size(); ++l) {
        const LegRayInfo& ray = _rays_info[l];
        if (!ray.hit) {
            continue;
        }
        const FloorIKData::LegSetup& leg = data.legs[l];

        // Updates leg joint chain so ankle reaches its targeted position.
        if (!_applyLegTwoBoneIK(data, leg, _ankles_target_ws[l], inv_root)) {
            return false;
        }

        // Updates leg joints model-space transforms.
        // Update will go from hip to ankle. Ankle's siblings might not be updated
        // as local-to-model will stop as soon as ankle joint is reached.
        _ltm_job.from = leg.hip;
        _ltm_job.to = leg.ankle;
        if (!_ltm_job.Run()) {
            return false;
        }

        // Computes ankle orientation, so it's aligned to the floor normal.
        const Vector3F aim_ik_target(_ankles_target_ws[l] + ray.hit_normal);
        if (!_applyAnkleAimIK(data, leg, aim_ik_target, inv_root)) {
            return false;
        }

        // Updates model-space transformation now ankle local changes is done.
        // Ankle rotation has already been updated, but its siblings (or it's
        // parent siblings) might are not. So we local-to-model update must
        // be complete starting from hip.
        _ltm_job.from = leg.hip;
        _ltm_job.to = animation::Skeleton::kMaxJoints;
        if (!_ltm_job.Run()) {
            return false;
        }
    }
    return true;
}

bool Animator::_applyLegTwoBoneIK(const FloorIKData& data,
                                  const FloorIKData::LegSetup& _leg,
                                  const Vector3F& _target_ws,
                                  const simd_math::Float4x4& _inv_root) {
    // Target position and pole vectors must be in model space.
    const simd_math::SimdFloat4 target_ms = TransformPoint(_inv_root, simd_math::simd_float4::Load3PtrU(&_target_ws.x));
    const simd_math::SimdFloat4 pole_vector_ms = _models[_leg.knee].cols[1];

    // Builds two bone IK job.
    animation::IKTwoBoneJob ik_job;
    ik_job.target = target_ms;
    ik_job.pole_vector = pole_vector_ms;
    // Mid-axis (knee) is constant (usually), and arbitrary defined by
    // skeleton/rig setup.
    ik_job.mid_axis = data.kKneeAxis;
    ik_job.weight = data.weight;
    ik_job.soften = data.soften;
    ik_job.start_joint = &_models[_leg.hip];
    ik_job.mid_joint = &_models[_leg.knee];
    ik_job.end_joint = &_models[_leg.ankle];
    simd_math::SimdQuaternion start_correction{};
    ik_job.start_joint_correction = &start_correction;
    simd_math::SimdQuaternion mid_correction{};
    ik_job.mid_joint_correction = &mid_correction;
    if (!ik_job.Run()) {
        return false;
    }
    // Apply IK quaternions to their respective local-space transforms.
    // Model-space transformations needs to be updated after a call to this
    // function.
    _multiplySoATransformQuaternion(_leg.hip, start_correction, make_span(_locals));
    _multiplySoATransformQuaternion(_leg.knee, mid_correction, make_span(_locals));

    return true;
}

bool Animator::_applyAnkleAimIK(const FloorIKData& data,
                                const FloorIKData::LegSetup& _leg,
                                const Vector3F& _target_ws,
                                const simd_math::Float4x4& _inv_root) {
    // Target position and pole vectors must be in model space.
    const simd_math::SimdFloat4 target_ms = TransformPoint(_inv_root, simd_math::simd_float4::Load3PtrU(&_target_ws.x));

    animation::IKAimJob ik_job;
    // Forward and up vectors are constant (usually), and arbitrary defined by
    // skeleton/rig setup.
    ik_job.forward = data.kAnkleForward;
    ik_job.up = data.kAnkleUp;

    // Model space targeted direction (floor normal in this case).
    ik_job.target = target_ms;

    // Uses constant ankle Y (skeleton/rig setup dependent) as pole vector. That
    // allows to maintain foot direction.
    ik_job.pole_vector = _models[_leg.ankle].cols[1];

    ik_job.joint = &_models[_leg.ankle];
    ik_job.weight = data.weight;
    simd_math::SimdQuaternion correction{};
    ik_job.joint_correction = &correction;
    if (!ik_job.Run()) {
        return false;
    }
    // Apply IK quaternions to their respective local-space transforms.
    // Model-space transformations needs to be updated after a call to this
    // function.
    _multiplySoATransformQuaternion(_leg.ankle, correction, make_span(_locals));

    return true;
}

}  // namespace vox