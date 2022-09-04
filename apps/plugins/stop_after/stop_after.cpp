//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/stop_after/stop_after.h"

namespace plugins {
StopAfter::StopAfter()
    : StopAfterTags("Stop After X",
                    "A collection of flags to stop the running application after a set period.",
                    {vox::Hook::ON_UPDATE},
                    {&stop_after_frame_flag_}) {}

bool StopAfter::IsActive(const vox::CommandParser &parser) { return parser.Contains(&stop_after_frame_flag_); }

void StopAfter::Init(const vox::CommandParser &parser) {
    remaining_frames_ = parser.As<uint32_t>(&stop_after_frame_flag_);
}

void StopAfter::OnUpdate(float delta_time) {
    remaining_frames_--;

    if (remaining_frames_ <= 0) {
        platform_->Close();
    }
}
}  // namespace plugins
