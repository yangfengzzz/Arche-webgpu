//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <utility>

#include "vox.base/memory/allocator.h"

namespace vox {

// Default deleter for vox unique_ptr, uses redirected memory allocator.
template <typename Ty>
struct Deleter {
    Deleter() = default;

    template <class Up>
    Deleter(const Deleter<Up>&, Ty* = nullptr) {}

    void operator()(Ty* _ptr) const { vox::Delete(_ptr); }
};

// Defines vox::unique_ptr to use vox default deleter.
template <typename Ty, typename Deleter = vox::Deleter<Ty>>
using unique_ptr = std::unique_ptr<Ty, Deleter>;

// Implements make_unique to use vox redirected memory allocator.
template <typename Ty, typename... Args>
unique_ptr<Ty> make_unique(Args&&... _args) {
    return unique_ptr<Ty>(New<Ty>(std::forward<Args>(_args)...));
}
}  // namespace vox