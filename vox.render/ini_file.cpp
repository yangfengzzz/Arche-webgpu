//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ini_file.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace vox {
namespace fs {
IniFile::IniFile(const std::string &p_filePath) : _filePath(p_filePath) {
    _load();
}

void IniFile::reload() {
    removeAll();
    _load();
}

bool IniFile::remove(const std::string &p_key) {
    if (isKeyExisting(p_key)) {
        _data.erase(p_key);
        return true;
    }
    
    return false;
}

void IniFile::removeAll() {
    _data.clear();
}

bool IniFile::isKeyExisting(const std::string &p_key) const {
    return _data.find(p_key) != _data.end();
}

void IniFile::_registerPair(const std::string &p_key, const std::string &p_value) {
    _registerPair(std::make_pair(p_key, p_value));
}

void IniFile::_registerPair(const AttributePair &p_pair) {
    _data.insert(p_pair);
}

std::vector<std::string> IniFile::getFormattedContent() const {
    std::vector<std::string> result;
    
    for (const auto&[key, value]: _data)
        result.push_back(key + "=" + value);
    
    return result;
}

void IniFile::_load() {
    std::fstream iniFile;
    iniFile.open(_filePath);
    
    if (iniFile.is_open()) {
        std::string currentLine;
        
        while (std::getline(iniFile, currentLine)) {
            if (_isValidLine(currentLine)) {
                currentLine.erase(std::remove_if(currentLine.begin(), currentLine.end(), isspace), currentLine.end());
                _registerPair(_extractKeyAndValue(currentLine));
            }
        }
        
        iniFile.close();
    }
}

void IniFile::rewrite() const {
    std::ofstream outfile;
    outfile.open(_filePath, std::ios_base::trunc);
    
    if (outfile.is_open()) {
        for (const auto&[key, value]: _data)
            outfile << key << "=" << value << std::endl;
    }
    
    outfile.close();
}

std::pair<std::string, std::string> IniFile::_extractKeyAndValue(const std::string &p_line) const {
    std::string key;
    std::string value;
    
    std::string *currentBuffer = &key;
    
    for (auto &c: p_line) {
        if (c == '=')
            currentBuffer = &value;
        else
            currentBuffer->push_back(c);
    }
    
    return std::make_pair(key, value);
}

bool IniFile::_isValidLine(const std::string &p_attributeLine) const {
    if (p_attributeLine.size() == 0)
        return false;
    
    if (p_attributeLine[0] == '#' || p_attributeLine[0] == ';' || p_attributeLine[0] == '[')
        return false;
    
    if (std::count(p_attributeLine.begin(), p_attributeLine.end(), '=') != 1)
        return false;
    
    return true;
}

bool IniFile::_stringToBoolean(const std::string &p_value) const {
    return (p_value == "1" || p_value == "T" || p_value == "t" || p_value == "True" || p_value == "true");
}

}
}
