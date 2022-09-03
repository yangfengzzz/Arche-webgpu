// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/grid_emitter2.h"

using namespace vox;

GridEmitter2::GridEmitter2() = default;

GridEmitter2::~GridEmitter2() = default;

void GridEmitter2::update(double currentTimeInSeconds, double timeIntervalInSeconds) {
    if (_onBeginUpdateCallback) {
        _onBeginUpdateCallback(currentTimeInSeconds, timeIntervalInSeconds);
    }

    onUpdate(currentTimeInSeconds, timeIntervalInSeconds);
}

bool GridEmitter2::isEnabled() const { return _isEnabled; }

void GridEmitter2::setIsEnabled(bool enabled) { _isEnabled = enabled; }

void GridEmitter2::setOnBeginUpdateCallback(const OnBeginUpdateCallback& callback) {
    _onBeginUpdateCallback = callback;
}
