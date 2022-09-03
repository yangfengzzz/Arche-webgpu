//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <string>
#include <typeindex>
#include <vector>

namespace vox {
class Plugin;

/**
 * @brief Abstract wrapper for implementing lists of commands. Useful for the CommandParser
 */
class Command {
public:
    Command(std::string name, std::string help_line);

    virtual ~Command() = default;

    /**
     * @brief Check that the command can be casted to another type of command
     *
     * @tparam U The type to cast to
     * @return true Can be casted
     * @return false Can not be casted
     */
    template <class U>
    [[nodiscard]] bool Is() const {
        return IsImpl(typeid(U));
    }

    /**
     * @brief Case to a specific type of command
     *
     * @tparam U The type of command to cast to
     * @return U* A ptr to this as the type of command
     */
    template <class U>
    U *Get() {
        assert(Is<U>() && "Attempting to retrieve incorrect command type");
        return static_cast<U *>(this);
    }

    /**
     * @brief Case to a specific type of command
     *
     * @tparam U The type of command to cast to
     * @return const U* A ptr to this as the type of command
     */
    template <class U>
    const U *Get() const {
        assert(Is<U>() && "Attempting to retrieve incorrect command type");
        return static_cast<const U *>(this);
    }

    [[nodiscard]] const std::string &GetName() const;

    void SetName(const std::string &name);

    [[nodiscard]] const std::string &GetHelpLine() const;

    void SetHelpLine(const std::string &help_line);

protected:
    /**
     * @brief Implementation of the is method. See TypedCommand
     *
     * @param index The type index to compare against
     * @return true Is equal to the given index
     * @return false Is not the equal to the given index
     */
    [[nodiscard]] virtual bool IsImpl(const std::type_index &index) const = 0;

private:
    std::string name_;
    std::string help_line_;
};

/**
 * @brief Allow a given command to implement subcommands
 */
class MultipleCommands {
public:
    explicit MultipleCommands(std::vector<Command *> commands);

    virtual ~MultipleCommands() = default;

    [[nodiscard]] const std::vector<Command *> &GetCommands() const;

private:
    std::vector<Command *> commands_;
};

/**
 * @brief Implement type logic through is<CommandType>
 *
 * @tparam Type The type of the command
 */
template <class Type>
class TypedCommand : public Command {
public:
    TypedCommand(const std::string &name, const std::string &help_line) : Command(name, help_line) {}

    ~TypedCommand() override = default;

protected:
    [[nodiscard]] bool IsImpl(const std::type_index &index) const override { return type_ == index; }

private:
    std::type_index type_ = std::type_index(typeid(Type));
};

/**
 * @brief Command groups allow separate commands to be shown in a labeled group
 */
class CommandGroup : public TypedCommand<CommandGroup>, public MultipleCommands {
public:
    CommandGroup(const std::string &name, const std::vector<Command *> &commands);

    ~CommandGroup() override = default;
};

/**
 * @brief Subcommands act as separate entrypoint to the application and may implement a subset of commands
 */
class SubCommand : public TypedCommand<SubCommand>, public MultipleCommands {
public:
    SubCommand(const std::string &name, const std::string &help_line, const std::vector<Command *> &commands);

    ~SubCommand() override = default;
};

/**
 * @brief Positional commands are values taken from a given position in the command string
 */
class PositionalCommand : public TypedCommand<PositionalCommand> {
public:
    PositionalCommand(const std::string &name, const std::string &help_line);

    ~PositionalCommand() override = default;
};

enum class FlagType {
    FLAG_ONLY,
    ONE_VALUE,
    MANY_VALUES,
};

/**
 * @brief Flag command represent a flag and value e.g --sample afbc
 */
class FlagCommand : public TypedCommand<FlagCommand> {
public:
    FlagCommand(FlagType type,
                const std::string &long_name,
                const std::string &short_name,
                const std::string &help_line);

    ~FlagCommand() override = default;

    [[nodiscard]] FlagType GetFlagType() const;

private:
    FlagType type_;
};

/**
 * @brief Abstract context which different command parsers may use to pass their own specialized contexts
 */
class CommandParserContext {
public:
    CommandParserContext() = default;

    virtual ~CommandParserContext() = default;
};

/**
 * @brief Visitor stype command parser used to construct a CLI and parse command line arguments
 */
class CommandParser {
public:
    virtual ~CommandParser() = default;

    virtual bool Contains(Command *command) const = 0;

    /**
     * @brief Cast a commands value to a given type
     *
     * @tparam Type Type to cast to
     * @param command Pointer to the command that should be casted
     * @return Type A cast version of the commands underlying value or a default initialization
     */
    template <typename Type>
    Type As(Command *command) const {
        auto values = GetCommandValue(command);
        Type type{};
        bool implemented_type_conversion = ConvertType(values, &type);
        assert(implemented_type_conversion && "Failed to retrieve value. Type unsupported");
        return type;
    }

    /**
     * @brief Retrieve the help menu generated by a parser implementation
     *
     * @return std::vector<std::string> A list of individual lines
     */
    [[nodiscard]] virtual std::vector<std::string> Help() const = 0;

    virtual bool Parse(const std::vector<Plugin *> &plugins) = 0;

    virtual bool Parse(const std::vector<Command *> &commands) = 0;

protected:
    /*
     * Individual parse functions visit each type of command to configure the underlying CLI implementation
     */
    virtual bool Parse(CommandParserContext *context, const std::vector<Command *> &commands);

    virtual void Parse(CommandParserContext *context, CommandGroup *command) = 0;

    virtual void Parse(CommandParserContext *context, SubCommand *command) = 0;

    virtual void Parse(CommandParserContext *context, PositionalCommand *command) = 0;

    virtual void Parse(CommandParserContext *context, FlagCommand *command) = 0;

private:
    /**
     * @brief Get the raw value parsed from command line arguments
     *
     * @param command The command to retrieve the raw value from
     * @return std::vector<std::string> The raw values for a given command
     */
    virtual std::vector<std::string> GetCommandValue(Command *command) const = 0;

    /**
     * @brief Cast from the CLI raw value to a given type
     *
     * @tparam Type Type required
     * @param values The raw CLI values
     * @param type The required type to cast to
     * @return true if implemented
     * @return false if no implementation exists
     */
    template <typename Type>
    inline bool ConvertType(const std::vector<std::string> &values, Type *type) const {
        return false;
    }
};

template <>
inline bool CommandParser::ConvertType(const std::vector<std::string> &values, uint32_t *type) const {
    if (values.size() != 1) {
        *type = 0;
    } else {
        auto number = atoi(values[0].c_str());
        *type = static_cast<uint32_t>(number);
    }
    return true;
}

template <>
inline bool CommandParser::ConvertType(const std::vector<std::string> &values, float *type) const {
    if (values.size() != 1) {
        *type = 0.0f;
    } else {
        *type = std::stof(values[0]);
    }
    return true;
}

template <>
inline bool CommandParser::ConvertType(const std::vector<std::string> &values, std::vector<std::string> *type) const {
    *type = values;
    return true;
}

template <>
inline bool CommandParser::ConvertType(const std::vector<std::string> &values, std::string *type) const {
    if (!values.empty()) {
        *type = values[0];
    } else {
        *type = "";
    }

    return true;
}

}  // namespace vox
