//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/benchmark_mode/benchmark_mode.h"

#include "vox.render/platform/platform.h"

namespace plugins {
BenchmarkMode::BenchmarkMode()
    : BenchmarkModeTags("Benchmark Mode",
                        "Log frame averages after running an app.",
                        {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START, vox::Hook::ON_APP_CLOSE},
                        {&benchmark_flag_}) {}

bool BenchmarkMode::IsActive(const vox::CommandParser &parser) { return parser.Contains(&benchmark_flag_); }

void BenchmarkMode::Init(const vox::CommandParser &parser) {
    // Whilst in benchmark mode fix the fps so that separate runs are consistently simulated
    // This will affect the graph outputs of framerate
    platform_->ForceSimulationFps(60.0f);
}

void BenchmarkMode::OnUpdate(float delta_time) {
    elapsed_time_ += delta_time;
    total_frames_++;
}

void BenchmarkMode::OnAppStart(const std::string &app_id) {
    elapsed_time_ = 0;
    total_frames_ = 0;
    LOGI("Starting Benchmark for {}", app_id)
}

void BenchmarkMode::OnAppClose(const std::string &app_id) {
    LOGI("Benchmark for {} completed in {} seconds (ran {} frames, averaged {} fps)", app_id, elapsed_time_,
         total_frames_, total_frames_ / elapsed_time_)
}
}  // namespace plugins
