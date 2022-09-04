//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/plugins.h"

#include <memory>

#include "apps/plugins/benchmark_mode/benchmark_mode.h"
#include "apps/plugins/file_logger/file_logger.h"
#include "apps/plugins/fps_logger/fps_logger.h"
#include "apps/plugins/screenshot/screenshot.h"
#include "apps/plugins/stop_after/stop_after.h"
#include "apps/plugins/window_options/window_options.h"

namespace plugins {

#define ADD_PLUGIN(name) plugins.emplace_back(std::make_unique<name>())

std::vector<vox::Plugin *> GetAll() {
    static bool once = true;
    static std::vector<std::unique_ptr<vox::Plugin>> plugins;

    if (once) {
        once = false;
        ADD_PLUGIN(BenchmarkMode);
        ADD_PLUGIN(FileLogger);
        ADD_PLUGIN(FpsLogger);
        ADD_PLUGIN(Screenshot);
        ADD_PLUGIN(StopAfter);
        ADD_PLUGIN(WindowOptions);
    }

    std::vector<vox::Plugin *> ptrs;
    ptrs.reserve(plugins.size());

    for (auto &plugin : plugins) {
        ptrs.push_back(plugin.get());
    }

    return ptrs;
}
}  // namespace plugins
