//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <vector>

namespace vox::nav {
void scanDirectoryAppend(const std::string& path, const std::string& ext, std::vector<std::string>& fileList);
void scanDirectory(const std::string& path, const std::string& ext, std::vector<std::string>& fileList);
}  // namespace vox::nav