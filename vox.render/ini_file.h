//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ini_file_hpp
#define ini_file_hpp

#include <string>
#include <unordered_map>

namespace vox {
namespace fs {
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
    IniFile(const std::string &p_filePath);
    
    /**
     * Overwrite the content of the current data by reloading the file
     */
    void reload();
    
    /**
     * Rewrite the entiere .ini file with the current values. This operation is destructive and can't be undone.
     * Any comment or line break in your .ini file will get destroyed
     */
    void rewrite() const;
    
    /**
     * Return the value attached to the given key
     * If the key doesn't exist, a default value is returned (0, false, "NULL")
     */
    template<typename T>
    T get(const std::string &p_key);
    
    /**
     * Return the value attached to the given key
     * If the key doesn't exist, the specified value is returned
     */
    template<typename T>
    T getOrDefault(const std::string &p_key, T p_default);
    
    /**
     * Set a new value to the given key (Not applied to the real file untill Rewrite() or Save() is called)
     */
    template<typename T>
    bool set(const std::string &p_key, const T &p_value);
    
    /**
     * Add a new key/value to the IniFile object (Not applied to the real file untill Rewrite() or Save() is called)
     */
    template<typename T>
    bool add(const std::string &p_key, const T &p_value);
    
    /**
     * Remove an key/value pair identified by the given key (Not applied to the real file untill Rewrite() or Save() is called)
     */
    bool remove(const std::string &p_key);
    
    /**
     * Remove all key/value pairs (Not applied to the real file untill Rewrite() or Save() is called)
     */
    void removeAll();
    
    /**
     * Verify if the given key exists
     */
    bool isKeyExisting(const std::string &p_key) const;
    
    /**
     * Get the content stored in the ini file as a vector of strings (Each string correspond to an attribute pair : Attribute=Value
     */
    std::vector<std::string> getFormattedContent() const;
    
private:
    void _registerPair(const std::string &p_key, const std::string &p_value);
    
    void _registerPair(const AttributePair &p_pair);
    
    void _load();
    
    AttributePair _extractKeyAndValue(const std::string &p_attributeLine) const;
    
    bool _isValidLine(const std::string &p_attributeLine) const;
    
    bool _stringToBoolean(const std::string &p_value) const;
    
private:
    std::string _filePath;
    AttributeMap _data;
};

}
}
#include "ini_file-inl.h"

#endif /* ini_file_hpp */
