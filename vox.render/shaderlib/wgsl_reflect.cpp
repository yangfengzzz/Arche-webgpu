//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_reflect.h"
#include <tint/tint.h>

namespace vox {
WGSLReflect::WGSLReflect(const std::string& filename) {
    tint::Source::File file("", "");
    auto p = std::make_unique<tint::Program>(tint::reader::wgsl::Parse(&file));
    tint::inspector::Inspector inspector(p.get());
    auto entry = inspector.GetEntryPoints();
    for (uint32_t i = 0; i < entry.size(); i++) {
        std::cout<<entry[i].name<<"\n";
        auto bindings = inspector.GetResourceBindings(entry[i].name);
        for (uint32_t j = 0; j < bindings.size(); j++) {
            std::cout<<bindings[j].bind_group << "\t" << bindings[j].binding << "\n";
        }
        std::cout<<"-----------------\n";
    }
}

}
