//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <CLI/FormatterFwd.hpp>
#include <functional>
#include <unordered_map>

namespace vox {
class HelpFormatter : public CLI::Formatter {
public:
    struct Meta {
        std::string name;
        std::string description;
    };

    HelpFormatter() = default;

    HelpFormatter(const HelpFormatter &) = default;

    HelpFormatter(HelpFormatter &&) = default;

    std::string make_help(const CLI::App *, std::string, CLI::AppFormatMode) const override;

    std::string make_expanded(const CLI::App *sub) const override;

    void RegisterMeta(const CLI::App *command, const Meta &meta);

private:
    std::unordered_map<const CLI::App *, Meta> meta_;

    const Meta *GetMeta(const CLI::App *command) const;
};

}  // namespace vox
