//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifndef VOX_INCLUDE_PRIVATE_HEADER
#error "This header is private, it cannot be included from public headers."
#endif  // VOX_INCLUDE_PRIVATE_HEADER

#include <cassert>

#include "vox.base/containers/stack.h"
#include "vox.base/containers/vector.h"

namespace vox::animation::offline {

// Decimation algorithm based on Ramer–Douglas–Peucker.
// https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm
// _Track must have std::vector interface.
// Adapter must have the following interface:
// struct Adapter {
//  bool Decimable(const Key&) const;
//  Key Lerp(const Key& _left, const Key& _right, const Key& _ref) const;
//  float Distance(const Key& _a, const Key& _b) const;
// };
template <typename Track, typename Adapter>
void Decimate(const Track& _src, const Adapter& _adapter, float _tolerance, Track* _dest) {
    // Early out if not enough data.
    if (_src.size() < 2) {
        *_dest = _src;
        return;
    }

    // Stack of segments to process.
    typedef std::pair<size_t, size_t> Segment;
    vox::stack<Segment> segments;

    // Bit vector of all points to included.
    vox::vector<bool> included(_src.size(), false);

    // Pushes segment made from first and last points.
    segments.push(Segment(0, _src.size() - 1));
    included[0] = true;
    included[_src.size() - 1] = true;

    // Empties segments stack.
    while (!segments.empty()) {
        // Pops next segment to process.
        const Segment segment = segments.top();
        segments.pop();

        // Looks for the furthest point from the segment.
        float max = -1.f;
        size_t candidate = segment.first;
        typename Track::const_reference left = _src[segment.first];
        typename Track::const_reference right = _src[segment.second];
        for (size_t i = segment.first + 1; i < segment.second; ++i) {
            assert(!included[i] && "Included points should be processed once only.");
            typename Track::const_reference test = _src[i];
            if (!_adapter.Decimable(test)) {
                candidate = i;
                break;
            } else {
                const float distance = _adapter.Distance(_adapter.Lerp(left, right, test), test);
                if (distance > _tolerance && distance > max) {
                    max = distance;
                    candidate = i;
                }
            }
        }

        // If found, include the point and pushes the 2 new segments (before and
        // after the new point).
        if (candidate != segment.first) {
            included[candidate] = true;
            if (candidate - segment.first > 1) {
                segments.push(Segment(segment.first, candidate));
            }
            if (segment.second - candidate > 1) {
                segments.push(Segment(candidate, segment.second));
            }
        }
    }

    // Copy all included points.
    _dest->clear();
    for (size_t i = 0; i < _src.size(); ++i) {
        if (included[i]) {
            _dest->push_back(_src[i]);
        }
    }

    // Removes last key if constant.
    if (_dest->size() > 1) {
        typename Track::const_iterator end = _dest->end();
        typename Track::const_reference last = *(--end);
        typename Track::const_reference penultimate = *(--end);
        const float distance = _adapter.Distance(penultimate, last);
        if (_adapter.Decimable(last) && distance <= _tolerance) {
            _dest->pop_back();
        }
    }
}
}  // namespace vox::animation::offline