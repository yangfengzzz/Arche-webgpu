//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Recast.h>

#include "vox.navigation/perf_timer.h"

namespace vox::nav {
/// Recast nav context.
class NavContext : public rcContext {
public:
    NavContext();

protected:
    void doResetLog() override;
    void doLog(rcLogCategory category, const char* msg, int len) override;
    void doResetTimers() override;
    void doStartTimer(rcTimerLabel label) override;
    void doStopTimer(rcTimerLabel label) override;
    [[nodiscard]] int doGetAccumulatedTime(rcTimerLabel label) const override;

private:
    TimeVal m_startTime[RC_MAX_TIMERS]{};
    TimeVal m_accTime[RC_MAX_TIMERS]{};
};

}  // namespace vox::nav