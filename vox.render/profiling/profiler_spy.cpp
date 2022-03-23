//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "profiler_spy.h"

namespace vox {
ProfilerSpy::ProfilerSpy(const std::string &p_name) :
name(p_name),
start(std::chrono::steady_clock::now()) {
}

ProfilerSpy::~ProfilerSpy() {
    end = std::chrono::steady_clock::now();
    Profiler::save(*this);
}

}
