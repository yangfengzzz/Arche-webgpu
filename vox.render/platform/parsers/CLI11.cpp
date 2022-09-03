//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/parsers/CLI11.h"

#include <utility>

#include "vox.base/logging.h"
#include "vox.render/strings.h"

namespace vox {
Cli11CommandContext::Cli11CommandContext(CLI::App *cli, CLI11CommandContextState state)
    : CommandParserContext(), cli_11_(cli), state_(std::move(state)) {}

bool Cli11CommandContext::HasGroupName() const { return !state_.group_name.empty(); }

const std::string &Cli11CommandContext::GetGroupName() const { return state_.group_name; }

CLI11CommandContextState Cli11CommandContext::GetState() const { return state_; }

Cli11CommandParser::Cli11CommandParser(const std::string &name,
                                       const std::string &description,
                                       const std::vector<std::string> &args)
    : cli_11_{std::make_unique<CLI::App>(description, name)}, formatter_{std::make_shared<HelpFormatter>()} {
    cli_11_->formatter(formatter_);

    args_.resize(args.size());
    std::transform(args.begin(), args.end(), args_.begin(),
                   [](const std::string &string) -> char * { return const_cast<char *>(string.c_str()); });
}

std::vector<std::string> Cli11CommandParser::Help() const { return Split(cli_11_->help(), "\n"); }

// Helper to reduce duplication - throw should not occur as there should always be a valid context passed
#define CAST(type)                                                                                 \
    void Cli11CommandParser::Parse(CommandParserContext *context, type *command) {                 \
        Parse(context == nullptr ? throw : dynamic_cast<Cli11CommandContext *>(context), command); \
    }

CAST(CommandGroup)

CAST(SubCommand)

CAST(PositionalCommand)

CAST(FlagCommand)

#undef CAST

void Cli11CommandParser::Parse(Cli11CommandContext *context, CommandGroup *command) {
    auto state = context->GetState();
    state.group_name = command->GetName();
    Cli11CommandContext group_context(context->cli_11_, state);
    CommandParser::Parse(&group_context, command->GetCommands());
}

void Cli11CommandParser::Parse(Cli11CommandContext *context, SubCommand *command) {
    auto *subcommand = context->cli_11_->add_subcommand(command->GetName(), command->GetHelpLine());
    sub_commands_[command] = subcommand;
    subcommand->formatter(formatter_);
    Cli11CommandContext subcommand_context(subcommand, context->GetState());
    CommandParser::Parse(&subcommand_context, command->GetCommands());
}

void Cli11CommandParser::Parse(Cli11CommandContext *context, PositionalCommand *command) {
    auto *option = context->cli_11_->add_option(command->GetName(), command->GetHelpLine());

    options_.emplace(command, option);

    if (context->HasGroupName()) {
        option->group(context->GetGroupName());
    }
}

void Cli11CommandParser::Parse(Cli11CommandContext *context, FlagCommand *command) {
    CLI::Option *flag;

    switch (command->GetFlagType()) {
        case FlagType::FLAG_ONLY:
            flag = context->cli_11_->add_flag(command->GetName(), command->GetHelpLine());
            break;
        case FlagType::ONE_VALUE:
        case FlagType::MANY_VALUES:
            flag = context->cli_11_->add_option(command->GetName(), command->GetHelpLine());
            break;
    }

    options_.emplace(command, flag);

    if (context->HasGroupName()) {
        flag->group(context->GetGroupName());
    }
}

bool Cli11CommandParser::Contains(Command *command) const {
    {
        auto it = options_.find(command);

        if (it != options_.end()) {
            return it->second->count() > 0;
        }
    }

    {
        auto it = sub_commands_.find(command);

        if (it != sub_commands_.end()) {
            return it->second->count() > 0;
        }
    }

    return false;
}

std::vector<std::string> Cli11CommandParser::GetCommandValue(Command *command) const {
    auto it = options_.find(command);

    if (it == options_.end()) {
        return {};
    }

    return it->second->results();
}

/*

 To create a CLI composed of multiple interoperable plugins using CLI11, we must create a CLI11 app from each plugin.
 This acts as a group for the commands used in said plugin. Once we have groups for each plugin we can then nest
 them inside each other using the CLI11::App::add_subcommand() method.

 This is required as CLI11 does not allow the redefinition of the same flag. Within the same app context.

 */
bool Cli11CommandParser::Parse(const std::vector<Plugin *> &plugins) {
    // Generate all command groups
    for (auto plugin : plugins) {
        auto group = std::make_unique<CLI::App>();

        formatter_->RegisterMeta(group.get(), {plugin->GetName(), plugin->GetDescription()});

        Cli11CommandContext context(group.get());
        CommandParser::Parse(&context, plugin->GetCliCommands());

        option_groups_[plugin] = std::move(group);
    }

    // Associate correct command groups
    for (auto plugin : plugins) {
        auto plugin_cli = option_groups_[plugin];
        auto included_plugins = plugin->GetInclusions();
        auto commands = plugin->GetCliCommands();

        for (auto command : commands) {
            // Share flags and options with sub commands
            if (command->Is<SubCommand>()) {
                auto cli_11_sub_command = sub_commands_[command];

                for (auto included_plugin : included_plugins) {
                    cli_11_sub_command->add_subcommand(option_groups_[included_plugin]);
                }
            }
        }

        cli_11_->add_subcommand(plugin_cli);
    }

    return Cli11Parse(cli_11_.get());
}

bool Cli11CommandParser::Parse(const std::vector<Command *> &commands) {
    Cli11CommandContext context(cli_11_.get());
    if (!CommandParser::Parse(&context, commands)) {
        return false;
    }

    return Cli11Parse(cli_11_.get());
}

bool Cli11CommandParser::Cli11Parse(CLI::App *app) {
    try {
        args_.insert(args_.begin(), "vulkan_samples");
        app->parse(static_cast<int>(args_.size()), args_.data());
    } catch (const CLI::CallForHelp &e) {
        return false;
    } catch (const CLI::ParseError &e) {
        bool success = e.get_exit_code() == static_cast<int>(CLI::ExitCodes::Success);

        if (!success) {
            LOGE("CLI11 Parse Error: [{}] {}", e.get_name(), e.what())
            return false;
        }
    }

    return true;
}

}  // namespace vox
