//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/stats/frame_time_stats_provider.h"

namespace vox {
FrameTimeStatsProvider::FrameTimeStatsProvider(std::set<StatIndex> &requested_stats) {
    // We always, and only, support StatIndex::frame_times since it's handled directly by us.
    // Remove from requested set to stop other providers looking for it.
    requested_stats.erase(StatIndex::FRAME_TIMES);
}

bool FrameTimeStatsProvider::IsAvailable(StatIndex index) const {
    // We only support StatIndex::frame_times
    return index == StatIndex::FRAME_TIMES;
}

StatsProvider::Counters FrameTimeStatsProvider::Sample(float delta_time) {
    Counters res;
    // frame_times comes directly from delta_time
    res[StatIndex::FRAME_TIMES].result = delta_time;
    return res;
}

}  // namespace vox
