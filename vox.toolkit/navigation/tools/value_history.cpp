//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/tools/value_history.h"

#include <cstring>

namespace vox::nav {

ValueHistory::ValueHistory() : m_hsamples(0) {
    for (float& m_sample : m_samples) m_sample = 0;
}

float ValueHistory::getSampleMin() const {
    float val = m_samples[0];
    for (int i = 1; i < MAX_HISTORY; ++i)
        if (m_samples[i] < val) val = m_samples[i];
    return val;
}

float ValueHistory::getSampleMax() const {
    float val = m_samples[0];
    for (int i = 1; i < MAX_HISTORY; ++i)
        if (m_samples[i] > val) val = m_samples[i];
    return val;
}

float ValueHistory::getAverage() const {
    float val = 0;
    for (float m_sample : m_samples) val += m_sample;
    return val / (float)MAX_HISTORY;
}

void GraphParams::setRect(int ix, int iy, int iw, int ih, int ipad) {
    x = ix;
    y = iy;
    w = iw;
    h = ih;
    pad = ipad;
}

void GraphParams::setValueRange(float ivmin, float ivmax, int indiv, const char* iunits) {
    vmin = ivmin;
    vmax = ivmax;
    ndiv = indiv;
    strcpy(units, iunits);
}

}  // namespace vox::nav
