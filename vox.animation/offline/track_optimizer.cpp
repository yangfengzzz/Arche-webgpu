//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/track_optimizer.h"

#include <cassert>

// Internal include file
#define VOX_INCLUDE_PRIVATE_HEADER  // Allows to include private headers.
#include "vox.animation/offline/decimate.h"
#include "vox.animation/offline/raw_track.h"

// Needs runtime track to access TrackPolicy.
#include "vox.animation/runtime/track.h"

namespace vox::animation::offline {

// Setup default values (favoring quality).
TrackOptimizer::TrackOptimizer() : tolerance(1e-3f) {  // 1 mm.
}

namespace {

template <typename KeyFrame>
struct Adapter {
    typedef typename KeyFrame::ValueType ValueType;
    typedef typename animation::internal::TrackPolicy<ValueType> Policy;

    Adapter() = default;

    [[nodiscard]] bool Decimable(const KeyFrame& _key) const {
        // RawTrackInterpolation::kStep keyframes aren't optimized, as steps can't
        // be interpolated.
        return _key.interpolation != RawTrackInterpolation::kStep;
    }

    [[nodiscard]] KeyFrame Lerp(const KeyFrame& _left, const KeyFrame& _right, const KeyFrame& _ref) const {
        assert(Decimable(_ref));
        const float alpha = (_ref.ratio - _left.ratio) / (_right.ratio - _left.ratio);
        assert(alpha >= 0.f && alpha <= 1.f);
        return {_ref.interpolation, _ref.ratio, Policy::Lerp(_left.value, _right.value, alpha)};
    }

    [[nodiscard]] float Distance(const KeyFrame& _a, const KeyFrame& _b) const {
        return Policy::Distance(_a.value, _b.value);
    }
};

template <typename Track>
inline bool Optimize(float _tolerance, const Track& _input, Track* _output) {
    if (!_output) {
        return false;
    }
    // Reset output animation to default.
    *_output = Track();

    // Validate animation.
    if (!_input.Validate()) {
        return false;
    }

    // Copy name
    _output->name = _input.name;

    // Optimizes.
    const Adapter<typename Track::Keyframe> adapter;
    Decimate(_input.keyframes, adapter, _tolerance, &_output->keyframes);

    // Output animation is always valid though.
    return _output->Validate();
}
}  // namespace

bool TrackOptimizer::operator()(const RawFloatTrack& _input, RawFloatTrack* _output) const {
    return Optimize(tolerance, _input, _output);
}
bool TrackOptimizer::operator()(const RawFloat2Track& _input, RawFloat2Track* _output) const {
    return Optimize(tolerance, _input, _output);
}
bool TrackOptimizer::operator()(const RawFloat3Track& _input, RawFloat3Track* _output) const {
    return Optimize(tolerance, _input, _output);
}
bool TrackOptimizer::operator()(const RawFloat4Track& _input, RawFloat4Track* _output) const {
    return Optimize(tolerance, _input, _output);
}
bool TrackOptimizer::operator()(const RawQuaternionTrack& _input, RawQuaternionTrack* _output) const {
    return Optimize(1.f - std::cos(.5f * tolerance), _input, _output);
}
}  // namespace vox::animation::offline
