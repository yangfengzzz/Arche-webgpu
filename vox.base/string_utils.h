//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace vox::utilities {
inline void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ") {
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/** converts a double or a float to a string */
template <typename T>
inline std::string real2String(const T r) {
    std::string str = std::to_string(r);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    str.erase(str.find_last_not_of('.') + 1, std::string::npos);
    return str;
}

inline std::string to_upper(const std::string& str) {
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

}  // namespace vox::utilities
