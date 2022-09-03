//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.geometry/frame.h"

using namespace vox;

Frame::Frame() = default;

Frame::Frame(int newIndex, double newTimeIntervalInSeconds)
    : index(newIndex), timeIntervalInSeconds(newTimeIntervalInSeconds) {}

double Frame::timeInSeconds() const { return index * timeIntervalInSeconds; }

void Frame::advance() { ++index; }

void Frame::advance(int delta) { index += delta; }

Frame& Frame::operator++() {
    advance();
    return *this;
}

Frame Frame::operator++(int i) {
    Frame result = *this;
    advance();
    return result;
}
