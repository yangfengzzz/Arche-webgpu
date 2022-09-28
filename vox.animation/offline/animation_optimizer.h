//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.base/containers/map.h"

namespace vox::animation {

// Forward declare runtime skeleton type.
class Skeleton;
namespace offline {

// Forward declare offline animation type.
struct RawAnimation;

// Defines the class responsible for optimizing an offline raw animation
// instance. Optimization is performed using a key frame reduction technique. It
// deciamtes redundant / interpolable key frames, within error tolerances given
// as input. The optimizer takes into account for each joint the error
// generated on its whole child hierarchy. This allows for example to take into
// consideration the error generated on a finger when optimizing the shoulder. A
// small error on the shoulder can be magnified when propagated to the finger
// indeed.
// It's possible to override optimization settings for a joint. This implicitely
// have an effect on the whole chain, up to that joint. This allows for example
// to have aggressive optimization for a whole skeleton, except for the chain
// that leads to the hand if user wants it to be precise. Default optimization
// tolerances are set in order to favor quality over runtime performances and
// memory footprint.
class VOX_ANIMOFFLINE_DLL AnimationOptimizer {
public:
    // Initializes the optimizer with default tolerances (favoring quality).
    AnimationOptimizer();

    // Optimizes _input using *these parameters. _skeleton is required to evaluate
    // optimization error along joint hierarchy (see hierarchical_tolerance).
    // Returns true on success and fills _output animation with the optimized
    // version of _input animation.
    // *_output must be a valid RawAnimation instance.
    // Returns false on failure and resets _output to an empty animation.
    // See RawAnimation::Validate() for more details about failure reasons.
    bool operator()(const RawAnimation& _input, const Skeleton& _skeleton, RawAnimation* _output) const;

    // Optimization settings.
    struct Setting {
        // Default settings
        Setting()
            : tolerance(1e-3f),  // 1mm
              distance(1e-1f)    // 10cm
        {}

        Setting(float _tolerance, float _distance) : tolerance(_tolerance), distance(_distance) {}

        // The maximum error that an optimization is allowed to generate on a whole
        // joint hierarchy.
        float tolerance;

        // The distance (from the joint) at which error is measured (if bigger that
        // joint hierarchy). This allows to emulate effect on skinning.
        float distance;
    };

    // Global optimization settings. These settings apply to all joints of the
    // hierarchy, unless overriden by joint specific settings.
    Setting setting;

    // Per joint override of optimization settings.
    typedef vox::map<int, Setting> JointsSetting;
    JointsSetting joints_setting_override;
};
}  // namespace offline
}  // namespace vox::animation