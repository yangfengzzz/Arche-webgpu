//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_utils.h"

namespace vox {
namespace editor {
void openURL(std::string url) {
    std::string op = std::string("open ").append(url);
    system(op.c_str());
}

std::string generateId(uint32_t length) {
    std::string id;
    srand(static_cast<uint32_t>(time(NULL)));

    for (int i = 0; i < length; i++) {
        id += std::to_string(rand() % 9);
    }

    return id;
}

}
}
