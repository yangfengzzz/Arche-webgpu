//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/utils.h"

#include <queue>
#include <stdexcept>

namespace vox {
std::string GetExtension(const std::string &uri) {
    auto dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error{"Uri has no extension"};
    }

    return uri.substr(dot_pos + 1);
}

std::string ToSnakeCase(const std::string &text) {
    std::stringstream result;

    for (const auto kCh : text) {
        if (std::isalpha(kCh)) {
            if (std::isspace(kCh)) {
                result << "_";
            } else {
                if (std::isupper(kCh)) {
                    result << "_";
                }

                result << static_cast<char>(std::tolower(kCh));
            }
        } else {
            result << kCh;
        }
    }

    return result.str();
}

}  // namespace vox
