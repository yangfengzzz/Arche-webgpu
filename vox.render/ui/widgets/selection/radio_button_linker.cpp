//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/selection/radio_button_linker.h"

namespace vox::ui {
RadioButtonLinker::RadioButtonLinker() : DataWidget<int>(selected_) {}

void RadioButtonLinker::Link(RadioButton &radio_button) {
    radio_button.radio_id_ = available_radio_id_++;
    auto listener_id = radio_button.clicked_event_ +=
            std::bind(&RadioButtonLinker::OnRadioButtonClicked, this, std::placeholders::_1);

    if (radio_button.IsSelected() && selected_ == -1) selected_ = radio_button.radio_id_;

    radio_buttons_.emplace_back(listener_id, std::ref(radio_button));
}

void RadioButtonLinker::Unlink(RadioButton &radio_button) {
    auto it = std::find_if(radio_buttons_.begin(), radio_buttons_.end(),
                           [&radio_button](std::pair<ListenerId, std::reference_wrapper<RadioButton>> &pair) {
                               return &pair.second.get() == &radio_button;
                           });

    if (it != radio_buttons_.end()) {
        it->second.get().clicked_event_.RemoveListener(it->first);
        radio_buttons_.erase(it);
    }
}

int RadioButtonLinker::Selected() const { return selected_; }

void RadioButtonLinker::DrawImpl() {
    // The RadioButtonLinker is special, it has nothing to display :)
}

void RadioButtonLinker::OnRadioButtonClicked(int radio_id) {
    if (radio_id != selected_) {
        selected_ = radio_id;
        value_changed_event_.Invoke(selected_);
        NotifyChange();

        for (const auto &[kListener, kRadioButton] : radio_buttons_)
            kRadioButton.get().selected_ = kRadioButton.get().radio_id_ == selected_;
    }
}

}  // namespace vox::ui
