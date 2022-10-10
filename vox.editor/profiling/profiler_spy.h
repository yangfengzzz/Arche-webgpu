//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <chrono>
#include <string>

#include "vox.editor/profiling/profiler.h"

namespace vox {
/**
 * This macro allow the creation of profiler spies
 * Any spy will die and send data to the profiler at
 * the end of the scope where this macro get called
 */
#define PROFILER_SPY(name)                          \
    std::unique_ptr<ProfilerSpy> __profiler_spy__ = \
            Profiler::IsEnabled() ? std::make_unique<ProfilerSpy>(name) : nullptr

/**
 * A little informer that is created when PROFILER_SPY(name) is written.
 * It helps collecting information about methods durations for debugging
 * event in release
 */
struct ProfilerSpy final {
    /**
     * Create the profiler spy with the given name.
     */
    explicit ProfilerSpy(std::string name);

    /**
     * Destroy the profiler spy.
     * On destruction, his collected data will be saved in the profiler
     */
    ~ProfilerSpy();

    const std::string name;
    const std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
};

}  // namespace vox
