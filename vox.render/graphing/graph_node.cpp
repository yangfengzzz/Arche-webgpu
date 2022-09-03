//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/graphing/graph_node.h"

namespace vox::graphing {
Node::Node(size_t id, const char *title, const char *style, const nlohmann::json &data) {
    attributes_["id"] = id;
    attributes_["label"] = title;
    attributes_["data"] = data;
    attributes_["style"] = style;
}

}  // namespace vox::graphing
