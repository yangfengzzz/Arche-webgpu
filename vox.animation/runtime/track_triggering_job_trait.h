//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/export.h"

// Defines iterator traits required to use TrackTriggeringJob::Iterator
// with stl algorithms.
// This is a separate file from "track_triggering_job.h" to prevent everyone
// from including stl file <iterator>.

#include <iterator>

#include "vox.animation/runtime/track_triggering_job.h"

// Specializes std::iterator_traits.
namespace std {
template <>
struct iterator_traits<vox::animation::TrackTriggeringJob::Iterator> {
    typedef ptrdiff_t difference_type;
    typedef vox::animation::TrackTriggeringJob::Edge value_type;
    typedef const vox::animation::TrackTriggeringJob::Edge* pointer;
    typedef const vox::animation::TrackTriggeringJob::Edge& reference;
    typedef forward_iterator_tag iterator_category;
};
}  // namespace std