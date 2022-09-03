//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "vox.base/macros.h"

namespace vox {

template <typename RandomIterator, typename T>
void serialFill(const RandomIterator &begin, const RandomIterator &end, const T &value) {
    auto size = static_cast<size_t>(end - begin);
    serialFor(size_t(0), size, [begin, value](size_t i) { begin[i] = value; });
}

template <typename IndexType, typename Function>
void serialFor(IndexType beginIndex, IndexType endIndex, const Function &function) {
    for (IndexType i = beginIndex; i < endIndex; ++i) {
        function(i);
    }
}

template <typename IndexType, typename Function>
void serialFor(IndexType beginIndexX,
               IndexType endIndexX,
               IndexType beginIndexY,
               IndexType endIndexY,
               const Function &function) {
    for (IndexType j = beginIndexY; j < endIndexY; ++j) {
        for (IndexType i = beginIndexX; i < endIndexX; ++i) {
            function(i, j);
        }
    }
}

template <typename IndexType, typename Function>
void serialFor(IndexType beginIndexX,
               IndexType endIndexX,
               IndexType beginIndexY,
               IndexType endIndexY,
               IndexType beginIndexZ,
               IndexType endIndexZ,
               const Function &function) {
    for (IndexType k = beginIndexZ; k < endIndexZ; ++k) {
        for (IndexType j = beginIndexY; j < endIndexY; ++j) {
            for (IndexType i = beginIndexX; i < endIndexX; ++i) {
                function(i, j, k);
            }
        }
    }
}

template <typename RandomIterator, typename SortingFunction>
void serialSort(RandomIterator begin, RandomIterator end, const SortingFunction &sortingFunction) {
    std::sort(begin, end, sortingFunction);
}

template <typename RandomIterator>
void serialSort(RandomIterator begin, RandomIterator end) {
    serialSort(begin, end, std::less<typename RandomIterator::value_type>());
}

}  // namespace vox
