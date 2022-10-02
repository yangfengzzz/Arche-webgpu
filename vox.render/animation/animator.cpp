//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

#include "vox.animation/runtime/ik_aim_job.h"
#include "vox.animation/runtime/ik_two_bone_job.h"
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
        _ltm_job.input = make_span(_rootState->locals());
        (void)_ltm_job.Run();

        _locals = _rootState->locals();
        _ltm_job.input = make_span(_locals);
        for (const auto& data : _scheduleData) {
            switch (data.type) {
                case ScheduleType::LocalToModel:
                    _applyLocalToModel(*static_cast<LocalToModelData*>(data.ptr));
                    break;
                case ScheduleType::TargetIK:
                    _applyAimIK(*static_cast<AimIKData*>(data.ptr));
                    break;
                case ScheduleType::TwoBoneIK:
                    break;
            }
        }
        clearAllSchedule();

        Matrix4x4F localMatrix;
        for (const auto& entities : _entityBindingMap) {
            size_t index = entities.first;
            memcpy(localMatrix.data(), &_models[index].cols[0], sizeof(Matrix4x4F));
            for (auto& entity : entities.second) {
                entity->transform->setLocalMatrix(localMatrix);
            }
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

void Animator::scheduleAimIK(const int& target_joint,
                             const Vector3F& target_ws,
                             float weight,
                             const Vector3F& pelvis_offset) {
    AimIKData data;
    data.target_joint = target_joint;
    data.target_ws = target_ws;
    data.weight = weight;
    data.pelvis_offset = pelvis_offset;
    _targetIKData.push_back(data);

    ScheduleData scheduleData{};
    scheduleData.ptr = &_targetIKData.back();
    scheduleData.type = ScheduleType::TargetIK;
    _scheduleData.push_back(scheduleData);
}

void Animator::scheduleLocalToModel(int from, int to) {
    LocalToModelData data{};
    data.from = from;
    data.to = to;
    _ltmData.push_back(data);

    ScheduleData scheduleData{};
    scheduleData.ptr = &_targetIKData.back();
    scheduleData.type = ScheduleType::LocalToModel;
    _scheduleData.push_back(scheduleData);
}

void Animator::clearAllSchedule() {
    _scheduleData.clear();
    _targetIKData.clear();
    _ltmData.clear();
}

void Animator::_applyAimIK(const AimIKData& data) {
    const Vector3F offsetTranslation = data.pelvis_offset + avatar.root_translation;
    auto _inv_root = simd_math::Float4x4::Translation(simd_math::simd_float4::Load3PtrU(&offsetTranslation.x)) *
                     simd_math::Float4x4::FromEuler(simd_math::simd_float4::LoadX(avatar.root_yaw));
    // Target position and pole vectors must be in model space.
    const simd_math::SimdFloat4 target_ms =
            TransformPoint(_inv_root, simd_math::simd_float4::Load3PtrU(&data.target_ws.x));

    animation::IKAimJob ik_job;
    // Forward and up vectors are constant (usually), and arbitrary defined by
    // skeleton/rig setup.
    ik_job.forward = avatar.kAnkleForward;
    ik_job.up = avatar.kAnkleUp;

    // Model space targeted direction (floor normal in this case).
    ik_job.target = target_ms;

    // Uses constant ankle Y (skeleton/rig setup dependent) as pole vector. That
    // allows to maintain foot direction.
    ik_job.pole_vector = _models[data.target_joint].cols[1];

    ik_job.joint = &_models[data.target_joint];
    ik_job.weight = data.weight;
    simd_math::SimdQuaternion correction{};
    ik_job.joint_correction = &correction;
    if (!ik_job.Run()) {
        return;
    }
    // Apply IK quaternions to their respective local-space transforms.
    // Model-space transformations needs to be updated after a call to this
    // function.
    _multiplySoATransformQuaternion(data.target_joint, correction, make_span(_locals));
}

void Animator::_applyLocalToModel(const LocalToModelData& data) {
    // Updates model-space transformation now ankle local changes is done.
    // Ankle rotation has already been updated, but its siblings (or it's
    // parent siblings) might are not. So we local-to-model update must
    // be complete starting from hip.
    _ltm_job.from = data.from;
    _ltm_job.to = data.to;
    if (!_ltm_job.Run()) {
        return;
    }
}

}  // namespace vox