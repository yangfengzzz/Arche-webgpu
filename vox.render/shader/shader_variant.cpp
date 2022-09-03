//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_variant.h"

#include "vox.render/strings.h"

namespace vox {
ShaderVariant::ShaderVariant(std::string &&preamble, std::vector<std::string> &&processes)
    : processes_{std::move(processes)} {
    auto splits = Split(preamble, "\n");
    for (const std::string &split : splits) {
        preambles_.insert(split);
    }

    UpdateId();
}

size_t ShaderVariant::GetId() const { return id_; }

void ShaderVariant::UnionCollection(const ShaderVariant &left, const ShaderVariant &right, ShaderVariant &result) {
    std::set<std::string> collect;
    for (const std::string &def : left.GetProcesses()) {
        collect.insert(def);
    }

    for (const std::string &def : right.GetProcesses()) {
        collect.insert(def);
    }

    for (const std::string &def : collect) {
        std::string tmp_def(def, 1);
        result.AddDefine(tmp_def);
    }
}

void ShaderVariant::AddDefine(const std::string &def) {
    auto iter = std::find(processes_.begin(), processes_.end(), "D" + def);
    if (iter == processes_.end()) {
        processes_.push_back("D" + def);
    }

    std::string tmp_def = def;

    // The "=" needs to turn into a space
    size_t pos_equal = tmp_def.find_first_of('=');
    if (pos_equal != std::string::npos) {
        tmp_def[pos_equal] = ' ';
    }

    preambles_.insert("#define " + tmp_def + "\n");

    UpdateId();
}

void ShaderVariant::RemoveDefine(const std::string &def) {
    std::string process = "D" + def;
    processes_.erase(std::remove(processes_.begin(), processes_.end(), process), processes_.end());

    std::string tmp_def = def;
    // The "=" needs to turn into a space
    size_t pos_equal = tmp_def.find_first_of('=');
    if (pos_equal != std::string::npos) {
        tmp_def[pos_equal] = ' ';
    }
    tmp_def = "#define " + tmp_def + "\n";
    auto iter = preambles_.find(tmp_def);
    if (iter != preambles_.end()) {
        preambles_.erase(iter);
    }

    UpdateId();
}

void ShaderVariant::AddRuntimeArraySize(const std::string &runtime_array_name, size_t size) {
    if (runtime_array_sizes_.find(runtime_array_name) == runtime_array_sizes_.end()) {
        runtime_array_sizes_.insert({runtime_array_name, size});
    } else {
        runtime_array_sizes_[runtime_array_name] = size;
    }
}

void ShaderVariant::SetRuntimeArraySizes(const std::unordered_map<std::string, size_t> &sizes) {
    runtime_array_sizes_ = sizes;
}

std::string ShaderVariant::GetPreamble() const {
    std::string preamble;
    std::for_each(preambles_.begin(), preambles_.end(), [&](const std::string &p) { preamble += p; });
    return preamble;
}

const std::vector<std::string> &ShaderVariant::GetProcesses() const { return processes_; }

const std::unordered_map<std::string, size_t> &ShaderVariant::GetRuntimeArraySizes() const {
    return runtime_array_sizes_;
}

void ShaderVariant::Clear() {
    preambles_.clear();
    processes_.clear();
    runtime_array_sizes_.clear();
    UpdateId();
}

void ShaderVariant::UpdateId() {
    id_ = 0;
    std::for_each(preambles_.begin(), preambles_.end(),
                  [&](const std::string &preamble) { utility::hash_combine(id_, preamble); });
}

}  // namespace vox
