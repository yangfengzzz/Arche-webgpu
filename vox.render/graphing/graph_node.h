//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <nlohmann/json.hpp>

namespace vox::graphing {
class Node {
public:
    Node() = default;
    ;

    Node(size_t id, const char *title, const char *style = nullptr, const nlohmann::json &data = {});

    template <typename T>
    static std::uintptr_t HandleToUintptrT(T handle) {
        return reinterpret_cast<std::uintptr_t>(reinterpret_cast<void *>(handle));
    }

    nlohmann::json attributes_;
};

}  // namespace vox::graphing
