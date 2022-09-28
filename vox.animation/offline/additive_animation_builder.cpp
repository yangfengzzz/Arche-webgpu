//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/additive_animation_builder.h"

#include <cassert>

#include "vox.animation/offline/raw_animation.h"
#include "vox.math/scalable_transform.h"

namespace vox::animation::offline {

namespace {
template <typename RawTrack, typename RefType, typename MakeDelta>
void MakeDelta(const RawTrack& _src, const RefType& reference, const MakeDelta& _make_delta, RawTrack* _dest) {
    _dest->reserve(_src.size());

    // Early out if no key.
    if (_src.empty()) {
        return;
    }

    // Copy animation keys.
    for (size_t i = 0; i < _src.size(); ++i) {
        const typename RawTrack::value_type delta = {_src[i].time, _make_delta(reference, _src[i].value)};
        _dest->push_back(delta);
    }
}

Vector3F MakeDeltaTranslation(const Vector3F& _reference, const Vector3F& _value) { return _value - _reference; }

QuaternionF MakeDeltaRotation(const QuaternionF& _reference, const QuaternionF& _value) {
    return _value * _reference.conjugate();
}

Vector3F MakeDeltaScale(const Vector3F& _reference, const Vector3F& _value) { return _value / _reference; }
}  // namespace

// Setup default values (favoring quality).
AdditiveAnimationBuilder::AdditiveAnimationBuilder() = default;

bool AdditiveAnimationBuilder::operator()(const RawAnimation& _input, RawAnimation* _output) const {
    if (!_output) {
        return false;
    }
    // Reset output animation to default.
    *_output = RawAnimation();

    // Validate animation.
    if (!_input.Validate()) {
        return false;
    }

    // Rebuilds output animation.
    _output->name = _input.name;
    _output->duration = _input.duration;
    _output->tracks.resize(_input.tracks.size());

    for (size_t i = 0; i < _input.tracks.size(); ++i) {
        const RawAnimation::JointTrack& track_in = _input.tracks[i];
        RawAnimation::JointTrack& track_out = _output->tracks[i];

        const RawAnimation::JointTrack::Translations& translations = track_in.translations;
        const Vector3F ref_translation = !translations.empty() ? translations[0].value : Vector3F();

        const RawAnimation::JointTrack::Rotations& rotations = track_in.rotations;
        const QuaternionF ref_rotation = !rotations.empty() ? rotations[0].value : QuaternionF::makeIdentity();

        const RawAnimation::JointTrack::Scales& scales = track_in.scales;
        const Vector3F ref_scale = !scales.empty() ? scales[0].value : Vector3F(1, 1, 1);

        MakeDelta(translations, ref_translation, MakeDeltaTranslation, &track_out.translations);
        MakeDelta(rotations, ref_rotation, MakeDeltaRotation, &track_out.rotations);
        MakeDelta(scales, ref_scale, MakeDeltaScale, &track_out.scales);
    }

    // Output animation is always valid though.
    return _output->Validate();
}

bool AdditiveAnimationBuilder::operator()(const RawAnimation& _input,
                                          const span<const ScalableTransform>& _reference_pose,
                                          RawAnimation* _output) const {
    if (!_output) {
        return false;
    }

    // Reset output animation to default.
    *_output = RawAnimation();

    // Validate animation.
    if (!_input.Validate()) {
        return false;
    }

    // The reference pose must have at least the same number of
    // tracks as the raw animation.
    if (_input.num_tracks() > static_cast<int>(_reference_pose.size())) {
        return false;
    }

    // Rebuilds output animation.
    _output->name = _input.name;
    _output->duration = _input.duration;
    _output->tracks.resize(_input.tracks.size());

    for (size_t i = 0; i < _input.tracks.size(); ++i) {
        MakeDelta(_input.tracks[i].translations, _reference_pose[i].translation, MakeDeltaTranslation,
                  &_output->tracks[i].translations);
        MakeDelta(_input.tracks[i].rotations, _reference_pose[i].rotation, MakeDeltaRotation,
                  &_output->tracks[i].rotations);
        MakeDelta(_input.tracks[i].scales, _reference_pose[i].scale, MakeDeltaScale, &_output->tracks[i].scales);
    }

    // Output animation is always valid though.
    return _output->Validate();
}

}  // namespace vox::animation::offline
