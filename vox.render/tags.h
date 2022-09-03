//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <vector>

/**
 * @brief Used to represent a tag
 */
typedef void (*TagId)();

/**
 * @brief Tag acts as a unique identifier to categories objects
 *
 * Tags are uniquely defined using different type names. The easiest way of creating a new tag is to use an empty struct
 * struct TagName{};
 * struct DifferentTag{};
 * Tag<TagName>::ID == Tag<TagName>::member != Tag<DifferentTag>:ID
 *
 * @tparam TAGS A set of tags
 */
template <typename... TAGS>
class Tag {
public:
    Tag() { tags_ = {Tag<TAGS>::id_...}; }

    static void Member(){};

    /**
     * @brief Unique TagID for a given Tag<TagName>
     */
    constexpr static TagId id_ = &Member;

    static bool HasTag(TagId id) { return std::find(tags_.begin(), tags_.end(), id) != tags_.end(); }

    template <typename C>
    static bool HasTag() {
        return has_tag(Tag<C>::id_);
    }

    template <typename... C>
    static bool HasTags() {
        std::vector<TagId> query = {Tag<C>::id_...};
        bool res = true;
        for (auto id : query) {
            res &= HasTag(id);
        }
        return res;
    }

private:
    static std::vector<TagId> tags_;
};

#ifndef DOXYGEN_SKIP

template <typename... TAGS>
std::vector<TagId> Tag<TAGS...>::tags_;

#endif
