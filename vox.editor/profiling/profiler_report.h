//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <vector>

namespace vox {
/**
 * Contains profiling data about a running period
 */
struct ProfilerReport final {
    /**
     * Data about an action (Called method)
     */
    struct Action final {
        std::string name;
        double duration;
        double percentage;
        uint64_t calls;
    };

    double elapsed_time = 0.0;
    uint16_t working_threads = 0u;
    uint32_t elapsed_frames = 0u;
    std::vector<Action> actions{};
};

}  // namespace vox
