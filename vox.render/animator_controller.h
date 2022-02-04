//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef animatorController_hpp
#define animatorController_hpp

#include <ozz/animation/runtime/animation.h>

namespace vox {
// Utility class that helps with controlling animation playback time. Time is
// computed every update according to the dt given by the caller, playback speed
// and "play" state.
// Internally time is stored as a ratio in unit interval [0,1], as expected by
// ozz runtime animation jobs.
// OnGui function allows to tweak controller parameters through the application
// Gui.
class AnimatorController {
public:
    // Constructor.
    AnimatorController();
    
    // Sets animation current time.
    void setTimeRatio(float _time);
    
    // Gets animation current time.
    float timeRatio() const;
    
    // Gets animation time ratio of last update. Useful when the range between
    // previous and current frame needs to pe processed.
    float previousTimeRatio() const;
    
    // Sets playback speed.
    void setPlaybackSpeed(float speed) {
        _playbackSpeed = speed;
    }
    
    // Gets playback speed.
    float playbackSpeed() const {
        return _playbackSpeed;
    }
    
    // Sets loop modes. If true, animation time is always clamped between 0 and 1.
    void setLoop(bool loop) {
        _loop = loop;
    }
    
    // Gets loop mode.
    bool loop() const {
        return _loop;
    }
    
    // Updates animation time if in "play" state, according to playback speed and
    // given frame time _dt.
    // Returns true if animation has looped during update
    void update(const ozz::animation::Animation &animation, float dt);
    
    // Resets all parameters to their default value.
    void reset();
    
private:
    // Current animation time ratio, in the unit interval [0,1], where 0 is the
    // beginning of the animation, 1 is the end.
    float _timeRatio;
    
    // Time ratio of the previous update.
    float _previousTimeRatio;
    
    // Playback speed, can be negative in order to play the animation backward.
    float _playbackSpeed;
    
    // Animation play mode state: play/pause.
    bool _play;
    
    // Animation loop mode.
    bool _loop;
};
}

#endif /* animatorController_hpp */
