//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ini_file_inl_h
#define ini_file_inl_h

#include <assert.h>
#include "ini_file.h"

namespace vox {
namespace fs {
template<typename T>
inline T IniFile::get(const std::string &p_key) {
    if constexpr (std::is_same<bool, T>::value) {
        if (!isKeyExisting(p_key))
            return false;
        
        return _stringToBoolean(_data[p_key]);
    } else if constexpr (std::is_same<std::string, T>::value) {
        if (!isKeyExisting(p_key))
            return std::string("NULL");
        
        return _data[p_key];
    } else if constexpr (std::is_integral<T>::value) {
        if (!isKeyExisting(p_key))
            return static_cast<T>(0);
        
        return static_cast<T>(std::atoi(_data[p_key].c_str()));
    } else if constexpr (std::is_floating_point<T>::value) {
        if (!isKeyExisting(p_key))
            return static_cast<T>(0.0f);
        
        return static_cast<T>(std::atof(_data[p_key].c_str()));
    } else {
        // static_assert(false, "The given type must be : bool, integral, floating point or string");
        return T();
    }
}

template<typename T>
inline T IniFile::getOrDefault(const std::string &p_key, T p_default) {
    return isKeyExisting(p_key) ? get<T>(p_key) : p_default;
}

template<typename T>
inline bool IniFile::set(const std::string &p_key, const T &p_value) {
    if (isKeyExisting(p_key)) {
        if constexpr (std::is_same<bool, T>::value) {
            _data[p_key] = p_value ? "true" : "false";
        } else if constexpr (std::is_same<std::string, T>::value) {
            _data[p_key] = p_value;
        } else if constexpr (std::is_integral<T>::value) {
            _data[p_key] = std::to_string(p_value);
        } else if constexpr (std::is_floating_point<T>::value) {
            _data[p_key] = std::to_string(p_value);
        } else {
            // static_assert(false, "The given type must be : bool, integral, floating point or string");
        }
        
        return true;
    }
    
    return false;
}

template<typename T>
inline bool IniFile::add(const std::string &p_key, const T &p_value) {
    if (!isKeyExisting(p_key)) {
        if constexpr (std::is_same<bool, T>::value) {
            _registerPair(p_key, p_value ? "true" : "false");
        } else if constexpr (std::is_same<std::string, T>::value) {
            _registerPair(p_key, p_value);
        } else if constexpr (std::is_integral<T>::value) {
            _registerPair(p_key, std::to_string(p_value));
        } else if constexpr (std::is_floating_point<T>::value) {
            _registerPair(p_key, std::to_string(p_value));
        } else {
            // static_assert(false, "The given type must be : bool, integral, floating point or std::string");
        }
        
        return true;
    }
    
    return false;
}

}
}
#endif /* ini_file_inl_h */
