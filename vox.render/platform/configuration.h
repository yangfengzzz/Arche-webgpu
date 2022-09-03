//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace vox {
class Setting {
public:
    Setting() = default;

    Setting(Setting &&other) = default;

    virtual ~Setting() = default;

    virtual void Set() = 0;

    virtual std::type_index GetType() = 0;
};

class BoolSetting : public Setting {
public:
    BoolSetting(bool &handle, bool value);

    void Set() override;

    std::type_index GetType() override;

private:
    bool &handle_;

    bool value_;
};

class IntSetting : public Setting {
public:
    IntSetting(int &handle, int value);

    void Set() override;

    std::type_index GetType() override;

private:
    int &handle_;

    int value_;
};

class EmptySetting : public Setting {
public:
    EmptySetting() = default;

    void Set() override;

    std::type_index GetType() override;
};

using ConfigMap = std::map<uint32_t, std::unordered_map<std::type_index, std::vector<Setting *>>>;

/**
 * @brief A class that contains configuration data for a sample.
 */
class Configuration {
public:
    /**
     * @brief Constructor
     */
    Configuration() = default;

    /**
     * @brief Configures the settings in the current config
     */
    void Set();

    /**
     * @brief Increments the configuration count
     * @returns True if the current configuration iterator was incremented
     */
    bool Next();

    /**
     * @brief Resets the configuration to beginning
     */
    void Reset();

    /**
     * @brief Inserts a setting into the current configuration
     * @param config_index The configuration to insert the setting into
     * @param setting A setting to be inserted into the configuration
     */
    void InsertSetting(uint32_t config_index, std::unique_ptr<Setting> setting);

    /**
     * @brief Inserts a setting into the current configuration
     * @param config_index The configuration to insert the setting into
     * @param args A parameter pack containing the parameters to initialize a setting object
     */
    template <class T, class... A>
    void Insert(uint32_t config_index, A &&...args) {
        static_assert(std::is_base_of<Setting, T>::value, "T is not a type of setting.");

        InsertSetting(config_index, std::make_unique<T>(args...));
    }

protected:
    ConfigMap configs_;

    std::vector<std::unique_ptr<Setting>> settings_;

    ConfigMap::iterator current_configuration_;
};

}  // namespace vox
