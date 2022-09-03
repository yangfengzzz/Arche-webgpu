//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <CLI/CLI.hpp>
#include <unordered_map>

#include "vox.render/platform/parser.h"
#include "vox.render/platform/parsers/help_formatter.h"
#include "vox.render/platform/plugins/plugin.h"

namespace vox {
struct CLI11CommandContextState {
    std::string group_name;
};

class Cli11CommandContext : public CommandParserContext {
public:
    explicit Cli11CommandContext(CLI::App *cli, CLI11CommandContextState state = {});

    ~Cli11CommandContext() override = default;

    [[nodiscard]] bool HasGroupName() const;

    [[nodiscard]] const std::string &GetGroupName() const;

    [[nodiscard]] CLI11CommandContextState GetState() const;

    CLI::App *cli_11_;

private:
    CLI11CommandContextState state_;
};

class Cli11CommandParser : public CommandParser {
public:
    Cli11CommandParser(const std::string &name, const std::string &description, const std::vector<std::string> &args);

    ~Cli11CommandParser() override = default;

    [[nodiscard]] std::vector<std::string> Help() const override;

    bool Parse(const std::vector<Plugin *> &plugins) override;

    bool Parse(const std::vector<Command *> &commands) override;

protected:
#define CAST(type) virtual void Parse(CommandParserContext *context, type *command) override;

    CAST(CommandGroup);

    CAST(SubCommand);

    CAST(PositionalCommand);

    CAST(FlagCommand);
#undef CAST

    void Parse(Cli11CommandContext *context, CommandGroup *command);

    void Parse(Cli11CommandContext *context, SubCommand *command);

    void Parse(Cli11CommandContext *context, PositionalCommand *command);

    void Parse(Cli11CommandContext *context, FlagCommand *command);

    bool Contains(Command *command) const override;

private:
    std::vector<const char *> args_;

    std::unique_ptr<CLI::App> cli_11_;
    std::unordered_map<Command *, CLI::Option *> options_;
    std::unordered_map<Command *, CLI::App *> sub_commands_;
    std::unordered_map<Plugin *, std::shared_ptr<CLI::App>> option_groups_;
    std::shared_ptr<HelpFormatter> formatter_;

    std::vector<std::string> GetCommandValue(Command *command) const override;

    bool Cli11Parse(CLI::App *app);
};

}  // namespace vox
