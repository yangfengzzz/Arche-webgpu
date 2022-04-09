//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef random_hpp
#define random_hpp

#include <random>
#include "algorithm.h"

namespace vox {
class Random {
public:
    Random();
    
    Random(uint32_t seed);
    
    void setSeed(uint32_t seed);
    
    float randomf(float min = 0.0f, float max = 1.0f) const;
    
    ssize_t random(ssize_t min = 0, ssize_t max = RAND_MAX) const;
    
    bool fithyFifthy() const;
    
    float randomBinomial(float max = 1.0f) const;
    
    template<typename I>
    I randomElement(I begin, I end) const {
        const int n = distance(begin, end);
        next(begin, random(0, n - 1));
        return begin;
    }
    
    uint32_t seed() const;
    
private:
    uint32_t _seed;
    mutable std::default_random_engine _engine;
};

inline bool Random::fithyFifthy() const {
    return randomf() >= 0.5f;
}

inline uint32_t Random::seed() const {
    return _seed;
}

}
#endif /* random_hpp */
