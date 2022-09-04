//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/file_logger/file_logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace plugins {
FileLogger::FileLogger() : FileLoggerTags("File Logger", "Enable log output to a file.", {}, {&log_file_flag_}) {}

bool FileLogger::IsActive(const vox::CommandParser &parser) { return parser.Contains(&log_file_flag_); }

void FileLogger::Init(const vox::CommandParser &parser) {
    if (parser.Contains(&log_file_flag_)) {
        if (spdlog::default_logger()) {
            auto log_file_name = parser.As<std::string>(&log_file_flag_);
            spdlog::default_logger()->sinks().push_back(
                    std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_name, true));
        }
    }
}
}  // namespace plugins
