//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/configuration.h"

namespace vox {
BoolSetting::BoolSetting(bool &handle, bool value) : handle_{handle}, value_{value} {}

void BoolSetting::Set() { handle_ = value_; }

std::type_index BoolSetting::GetType() { return typeid(BoolSetting); }

IntSetting::IntSetting(int &handle, int value) : handle_{handle}, value_{value} {}

void IntSetting::Set() { handle_ = value_; }

std::type_index IntSetting::GetType() { return typeid(IntSetting); }

void EmptySetting::Set() {}

std::type_index EmptySetting::GetType() { return typeid(EmptySetting); }

void Configuration::Set() {
    for (const auto &pair : current_configuration_->second) {
        for (auto setting : pair.second) {
            setting->Set();
        }
    }
}

bool Configuration::Next() {
    if (configs_.empty()) {
        return false;
    }

    current_configuration_++;

    if (current_configuration_ == configs_.end()) {
        return false;
    }

    return true;
}

void Configuration::Reset() { current_configuration_ = configs_.begin(); }

void Configuration::InsertSetting(uint32_t config_index, std::unique_ptr<Setting> setting) {
    settings_.push_back(std::move(setting));
    configs_[config_index][settings_.back()->GetType()].push_back(settings_.back().get());
}

}  // namespace vox
