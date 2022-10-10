//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>

namespace vox::fs {
/**
 * The IniFile class represents a file .ini that stores a set of attributes/values that can get read and written
 */
class IniFile final {
public:
    using AttributePair = std::pair<std::string, std::string>;
    using AttributeMap = std::unordered_map<std::string, std::string>;

    /**
     * Create an IniFile by parsing the given file path and extracting key/values pairs for future usage
     */
    explicit IniFile(std::string p_file_path);

    /**
     * Overwrite the content of the current data by reloading the file
     */
    void Reload();

    /**
     * Rewrite the entire .ini file with the current values. This operation is destructive and can't be undone.
     * Any comment or line break in your .ini file will get destroyed
     */
    void Rewrite() const;

    /**
     * Return the value attached to the given key
     * If the key doesn't exist, a default value is returned (0, false, "NULL")
     */
    template <typename T>
    T Get(const std::string &p_key);

    /**
     * Return the value attached to the given key
     * If the key doesn't exist, the specified value is returned
     */
    template <typename T>
    T GetOrDefault(const std::string &p_key, T p_default);

    /**
     * Set a new value to the given key (Not applied to the real file until Rewrite() or Save() is called)
     */
    template <typename T>
    bool Set(const std::string &p_key, const T &p_value);

    /**
     * Add a new key/value to the IniFile object (Not applied to the real file until Rewrite() or Save() is called)
     */
    template <typename T>
    bool Add(const std::string &p_key, const T &p_value);

    /**
     * Remove an key/value pair identified by the given key (Not applied to the real file until Rewrite() or Save() is
     * called)
     */
    bool Remove(const std::string &p_key);

    /**
     * Remove all key/value pairs (Not applied to the real file until Rewrite() or Save() is called)
     */
    void RemoveAll();

    /**
     * Verify if the given key exists
     */
    [[nodiscard]] bool IsKeyExisting(const std::string &p_key) const;

    /**
     * Get the content stored in the ini file as a vector of strings (Each string correspond to an attribute pair :
     * Attribute=Value
     */
    [[nodiscard]] std::vector<std::string> GetFormattedContent() const;

private:
    void RegisterPair(const std::string &p_key, const std::string &p_value);

    void RegisterPair(const AttributePair &p_pair);

    void Load();

    [[nodiscard]] static AttributePair ExtractKeyAndValue(const std::string &p_attribute_line);

    [[nodiscard]] static bool IsValidLine(const std::string &p_attribute_line);

    [[nodiscard]] static bool StringToBoolean(const std::string &p_value);

private:
    std::string file_path_;
    AttributeMap data_;
};

}  // namespace vox::fs
#include "vox.editor/ini_file-inl.h"
