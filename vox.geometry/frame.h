//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.base/macros.h"

namespace vox {

//!
//! \brief Representation of an animation frame.
//!
//! This struct holds current animation frame index and frame interval in
//! seconds.
//!
struct Frame final {
    //! Frame index.
    int index = 0;

    //! Time interval in seconds between two adjacent frames.
    double timeIntervalInSeconds = 1.0 / 60.0;

    //! Constructs Frame instance with 1/60 seconds time interval.
    Frame();

    //! Constructs Frame instance with given time interval.
    Frame(int newIndex, double newTimeIntervalInSeconds);

    //! Returns the elapsed time in seconds.
    [[nodiscard]] double timeInSeconds() const;

    //! Advances single frame.
    void advance();

    //! Advances multiple frames.
    //! \param delta Number of frames to advance.
    void advance(int delta);

    //! Advances single frame (prefix).
    Frame& operator++();

    //! Advances single frame (postfix).
    Frame operator++(int);
};

}  // namespace vox
