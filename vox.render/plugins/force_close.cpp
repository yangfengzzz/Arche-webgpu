//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/plugins/force_close.h"

#include <iostream>

namespace vox::plugins {
ForceClose::ForceClose()
    : ForceCloseTags(
              "Force Close", "Force the application to close if it has been halted before exiting", {}, {&stop_cmd_}) {}

bool ForceClose::IsActive(const CommandParser &parser) { return parser.Contains(&stop_cmd_); }

void ForceClose::Init(const CommandParser &parser) {}

}  // namespace vox::plugins
