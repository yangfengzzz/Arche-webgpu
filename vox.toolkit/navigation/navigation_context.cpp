//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/navigation_context.h"

#include "vox.base/logging.h"

namespace vox::nav {
NavContext::NavContext() { resetTimers(); }

void NavContext::doResetLog() {}

void NavContext::doLog(const rcLogCategory category, const char* msg, const int len) {
    switch (category) {
        case rcLogCategory::RC_LOG_PROGRESS:
            LOGI(msg)
            break;
        case rcLogCategory::RC_LOG_ERROR:
            LOGE(msg)
            break;
        case rcLogCategory::RC_LOG_WARNING:
            LOGW(msg)
            break;
    }
}

void NavContext::doResetTimers() {
    for (long long& i : m_accTime) i = -1;
}

void NavContext::doStartTimer(const rcTimerLabel label) { m_startTime[label] = getPerfTime(); }

void NavContext::doStopTimer(const rcTimerLabel label) {
    const TimeVal endTime = getPerfTime();
    const TimeVal deltaTime = endTime - m_startTime[label];
    if (m_accTime[label] == -1)
        m_accTime[label] = deltaTime;
    else
        m_accTime[label] += deltaTime;
}

int NavContext::doGetAccumulatedTime(const rcTimerLabel label) const { return getPerfTimeUsec(m_accTime[label]); }

}  // namespace vox::nav