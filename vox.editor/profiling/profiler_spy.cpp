//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/profiling/profiler_spy.h"

#include <utility>

namespace vox {
ProfilerSpy::ProfilerSpy(std::string name) : name(std::move(name)), start(std::chrono::steady_clock::now()) {}

ProfilerSpy::~ProfilerSpy() {
    end = std::chrono::steady_clock::now();
    Profiler::Save(*this);
}

}  // namespace vox
