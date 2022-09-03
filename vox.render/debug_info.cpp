//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/debug_info.h"

namespace vox {
std::vector<std::unique_ptr<field::Base>> &DebugInfo::get_fields() { return fields_; }

float DebugInfo::get_longest_label() const {
    float column_width = 0.0f;
    for (auto &field : fields_) {
        const std::string &label = field->label;

        if (label.size() > column_width) {
            column_width = static_cast<float>(label.size());
        }
    }
    return column_width;
}

}  // namespace vox
