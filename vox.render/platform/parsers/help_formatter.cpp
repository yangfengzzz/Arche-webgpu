//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/parsers/help_formatter.h"

#include "vox.render/platform/parsers/CLI11.h"
#include "vox.render/strings.h"

namespace vox {
const std::string kSpacer = "  ";

std::string prepend(const std::string &target, const std::string &prepended) {
    auto lines = Split(target, "\n");

    if (lines.size() == 1) {
        if (!lines[0].empty()) {
            return prepended + lines[0];
        }
    } else if (lines.size() > 1) {
        return prepended + Join(lines, "\n" + prepended);
    }

    return target;
}

std::string HelpFormatter::make_help(const CLI::App *app, std::string name, CLI::AppFormatMode mode) const {
    // This immediately forwards to the make_expanded method. This is done this way so that subcommands can
    // have overridden formatters
    if (mode == CLI::AppFormatMode::Sub) return make_expanded(app);

    std::stringstream out;
    if ((app->get_name().empty()) && (app->get_parent() != nullptr)) {
        if (app->get_group() != "Subcommands") {
            out << app->get_group() << ':';
        }
    }

    out << make_description(app);
    out << make_usage(app, name);
    out << prepend(make_positionals(app), kSpacer);
    out << prepend(make_groups(app, mode), kSpacer);
    out << "\n" << prepend(make_subcommands(app, mode), kSpacer);

    return out.str();
}

std::string HelpFormatter::make_expanded(const CLI::App *sub) const {
#define SPACE()                                 \
    {                                           \
        size_t current_size = out.str().size(); \
        if (current_size > last_size) {         \
            last_size = current_size;           \
            out << "\n";                        \
        }                                       \
    }

    const auto *meta = GetMeta(sub);

    std::stringstream out;

    if (meta != nullptr) {
        out << meta->name << "\n";
    } else {
        out << sub->get_display_name(true) << "\n";
    }

    if (meta != nullptr) {
        out << kSpacer << meta->description << "\n";
    } else {
        out << prepend(make_description(sub), kSpacer);
    }

    if (sub->get_name().empty() && !sub->get_aliases().empty()) {
        CLI::detail::format_aliases(out, sub->get_aliases(), column_width_ + 2);
    }

    size_t last_size = out.str().size();

    out << prepend(make_positionals(sub), kSpacer);

    SPACE()

    out << prepend(make_groups(sub, CLI::AppFormatMode::Sub), kSpacer);

    SPACE()

    out << prepend(make_subcommands(sub, CLI::AppFormatMode::Sub), kSpacer);

    SPACE()

    return out.str();

#undef SPACE
}

void HelpFormatter::RegisterMeta(const CLI::App *command, const HelpFormatter::Meta &meta) {
    meta_.insert({command, meta});
}

const HelpFormatter::Meta *HelpFormatter::GetMeta(const CLI::App *command) const {
    auto it = meta_.find(command);
    return it == meta_.end() ? nullptr : &it->second;
}

}  // namespace vox
