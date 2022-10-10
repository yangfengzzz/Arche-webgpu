//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ini_file.h"

#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

namespace vox::fs {
IniFile::IniFile(std::string p_file_path) : file_path_(std::move(p_file_path)) { Load(); }

void IniFile::Reload() {
    RemoveAll();
    Load();
}

bool IniFile::Remove(const std::string &p_key) {
    if (IsKeyExisting(p_key)) {
        data_.erase(p_key);
        return true;
    }

    return false;
}

void IniFile::RemoveAll() { data_.clear(); }

bool IniFile::IsKeyExisting(const std::string &p_key) const { return data_.find(p_key) != data_.end(); }

void IniFile::RegisterPair(const std::string &p_key, const std::string &p_value) {
    RegisterPair(std::make_pair(p_key, p_value));
}

void IniFile::RegisterPair(const AttributePair &p_pair) { data_.insert(p_pair); }

std::vector<std::string> IniFile::GetFormattedContent() const {
    std::vector<std::string> result;

    for (const auto &[kEy, kValue] : data_) result.push_back(kEy + "=" + kValue);

    return result;
}

void IniFile::Load() {
    std::fstream ini_file;
    ini_file.open(file_path_);

    if (ini_file.is_open()) {
        std::string current_line;

        while (std::getline(ini_file, current_line)) {
            if (IsValidLine(current_line)) {
                current_line.erase(std::remove_if(current_line.begin(), current_line.end(), isspace),
                                   current_line.end());
                RegisterPair(ExtractKeyAndValue(current_line));
            }
        }

        ini_file.close();
    }
}

void IniFile::Rewrite() const {
    std::ofstream outfile;
    outfile.open(file_path_, std::ios_base::trunc);

    if (outfile.is_open()) {
        for (const auto &[kEy, kValue] : data_) outfile << kEy << "=" << kValue << std::endl;
    }

    outfile.close();
}

std::pair<std::string, std::string> IniFile::ExtractKeyAndValue(const std::string &p_attribute_line) {
    std::string key;
    std::string value;

    std::string *current_buffer = &key;

    for (auto &c : p_attribute_line) {
        if (c == '=')
            current_buffer = &value;
        else
            current_buffer->push_back(c);
    }

    return std::make_pair(key, value);
}

bool IniFile::IsValidLine(const std::string &p_attribute_line) {
    if (p_attribute_line.empty()) return false;

    if (p_attribute_line[0] == '#' || p_attribute_line[0] == ';' || p_attribute_line[0] == '[') return false;

    if (std::count(p_attribute_line.begin(), p_attribute_line.end(), '=') != 1) return false;

    return true;
}

bool IniFile::StringToBoolean(const std::string &p_value) {
    return (p_value == "1" || p_value == "T" || p_value == "t" || p_value == "True" || p_value == "true");
}

}  // namespace vox::fs
