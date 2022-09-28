//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/string_utils.h"

namespace vox {
bool strmatch(const char* _str, const char* _pattern) {
    for (; *_pattern; ++_str, ++_pattern) {
        if (*_pattern == '?') {
            if (!*_str) {
                return false;
            }
        } else if (*_pattern == '*') {
            if (strmatch(_str, _pattern + 1)) {
                return true;
            }
            if (*_str && strmatch(_str + 1, _pattern)) {
                return true;
            }
            return false;
        } else {
            if (*_str != *_pattern) {
                return false;
            }
        }
    }
    return !*_str && !*_pattern;
}
}  // namespace vox