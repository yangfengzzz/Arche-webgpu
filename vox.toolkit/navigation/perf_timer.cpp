//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/perf_timer.h"

#if defined(WIN32)
// Win32
#include <windows.h>
namespace vox::nav {
TimeVal getPerfTime() {
    __int64 count;
    QueryPerformanceCounter((LARGE_INTEGER*)&count);
    return count;
}

int getPerfTimeUsec(const TimeVal duration) {
    static __int64 freq = 0;
    if (freq == 0) QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    return (int)(duration * 1000000 / freq);
}
}  // namespace vox::nav
#else
// Linux, BSD, OSX
#include <sys/time.h>
namespace vox::nav {
TimeVal getPerfTime() {
    timeval now{};
    gettimeofday(&now, nullptr);
    return (TimeVal)now.tv_sec * 1000000L + (TimeVal)now.tv_usec;
}

int getPerfTimeUsec(const TimeVal duration) { return (int)duration; }
}  // namespace vox::nav
#endif
