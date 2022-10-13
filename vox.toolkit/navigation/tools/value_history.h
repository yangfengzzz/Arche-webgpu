//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::nav {

class ValueHistory {
public:
    static constexpr int MAX_HISTORY = 256;

    ValueHistory();

    inline void addSample(const float val) {
        m_hsamples = (m_hsamples + MAX_HISTORY - 1) % MAX_HISTORY;
        m_samples[m_hsamples] = val;
    }

    [[nodiscard]] inline float getSample(const int i) const { return m_samples[(m_hsamples + i) % MAX_HISTORY]; }

    [[nodiscard]] float getSampleMin() const;
    [[nodiscard]] float getSampleMax() const;
    [[nodiscard]] float getAverage() const;

private:
    float m_samples[MAX_HISTORY]{};
    int m_hsamples;
};

struct GraphParams {
    void setRect(int ix, int iy, int iw, int ih, int ipad);
    void setValueRange(float ivmin, float ivmax, int indiv, const char* iunits);

    int x, y, w, h, pad;
    float vmin, vmax;
    int ndiv;
    char units[16];
};

}  // namespace vox::nav