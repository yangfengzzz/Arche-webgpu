//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/parser.h"

#include <utility>

namespace vox {
Command::Command(std::string name, std::string help_line) : name_(std::move(name)), help_line_(std::move(help_line)) {}

const std::string &Command::GetName() const {
    assert(!name_.empty() && "Command name unset");
    return name_;
}

void Command::SetName(const std::string &name) { name_ = name; }

const std::string &Command::GetHelpLine() const {
    assert(!help_line_.empty() && "Command help line unset");
    return help_line_;
}

void Command::SetHelpLine(const std::string &help_line) { help_line_ = help_line; }

MultipleCommands::MultipleCommands(std::vector<Command *> commands) : commands_(std::move(commands)) {}

const std::vector<Command *> &MultipleCommands::GetCommands() const { return commands_; }

CommandGroup::CommandGroup(const std::string &name, const std::vector<Command *> &commands)
    : TypedCommand<CommandGroup>(name, ""), MultipleCommands(commands) {}

SubCommand::SubCommand(const std::string &name, const std::string &help_line, const std::vector<Command *> &commands)
    : TypedCommand<SubCommand>(name, help_line), MultipleCommands(commands) {}

PositionalCommand::PositionalCommand(const std::string &name, const std::string &help_line)
    : TypedCommand<PositionalCommand>(name, help_line) {}

FlagCommand::FlagCommand(FlagType type,
                         const std::string &long_name,
                         const std::string &short_name,
                         const std::string &help_line)
    : TypedCommand<FlagCommand>("", help_line), type_{type} {
    std::string name;

    if (!short_name.empty()) {
        name += "-" + short_name;
    }

    if (!long_name.empty()) {
        if (!name.empty()) {
            name += ",";
        }

        name += "--" + long_name;
    }

    Command::SetName(name);
}

FlagType FlagCommand::GetFlagType() const { return type_; }

bool CommandParser::Parse(const std::vector<Command *> &commands) { return Parse(nullptr, commands); }

bool CommandParser::Parse(CommandParserContext *context, const std::vector<Command *> &commands) {
#define PARSE(type)                           \
    if (command->Is<type>()) {                \
        Parse(context, command->Get<type>()); \
    }

    for (auto *command : commands) {
        PARSE(SubCommand)
        PARSE(PositionalCommand)
        PARSE(FlagCommand)
        PARSE(CommandGroup)
    }

#undef PARSE

    return true;
}

}  // namespace vox
