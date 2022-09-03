//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/filesystem.h"

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

VKBP_ENABLE_WARNINGS()

#include "vox.render/platform/platform.h"

namespace vox::fs {
namespace path {
const std::unordered_map<Type, std::string> kRelativePaths = {
        {Type::ASSETS, "assets/"},    {Type::SHADERS, "shaders/"},
        {Type::STORAGE, "output/"},   {Type::SCREENSHOTS, "output/images/"},
        {Type::LOGS, "output/logs/"}, {Type::GRAPHS, "output/graphs/"}};

std::string Get(const Type type, const std::string &file) {
    assert(kRelativePaths.size() == Type::TOTAL_RELATIVE_PATH_TYPES &&
           "Not all paths are defined in filesystem, please check that each enum is specified");

    // Check for special cases first
    if (type == Type::WORKING_DIR) {
        return Platform::GetExternalStorageDirectory();
    } else if (type == Type::TEMP) {
        return Platform::GetTempDirectory();
    }

    // Check for relative paths
    auto it = kRelativePaths.find(type);

    if (kRelativePaths.size() < Type::TOTAL_RELATIVE_PATH_TYPES) {
        throw std::runtime_error("Platform hasn't initialized the paths correctly");
    } else if (it == kRelativePaths.end()) {
        throw std::runtime_error("Path enum doesn't exist, or wasn't specified in the path map");
    } else if (it->second.empty()) {
        throw std::runtime_error("Path was found, but it is empty");
    }

    auto path = Platform::GetExternalStorageDirectory() + it->second;

    if (!IsDirectory(path)) {
        CreatePath(Platform::GetExternalStorageDirectory(), it->second);
    }

    return path + file;
}
}  // namespace path

bool IsDirectory(const std::string &path) {
    struct stat info {};
    if (stat(path.c_str(), &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        return false;
    }
}

bool IsFile(const std::string &filename) {
    std::ifstream f(filename.c_str());
    return !f.fail();
}

void CreatePath(const std::string &root, const std::string &path) {
    for (auto it = path.begin(); it != path.end(); ++it) {
        it = std::find(it, path.end(), '/');
        CreateDirectory(root + std::string(path.begin(), it));
    }
}

std::string ReadTextFile(const std::string &filename) {
    std::vector<std::string> data;

    std::ifstream file;

    file.open(filename, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    return std::string{(std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())};
}

std::vector<uint8_t> read_binary_file(const std::string &filename, const uint32_t count) {
    std::vector<uint8_t> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    data.resize(static_cast<size_t>(read_count));
    file.read(reinterpret_cast<char *>(data.data()), read_count);
    file.close();

    return data;
}

static void WriteBinaryFile(const std::vector<uint8_t> &data, const std::string &filename, const uint32_t count) {
    std::ofstream file;

    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    uint64_t write_count = count;
    if (count == 0) {
        write_count = data.size();
    }

    file.write(reinterpret_cast<const char *>(data.data()), write_count);
    file.close();
}

std::vector<uint8_t> ReadAsset(const std::string &filename, uint32_t count) {
    return read_binary_file(path::Get(path::Type::ASSETS) + filename, count);
}

std::string ReadShader(const std::string &filename) { return ReadTextFile(path::Get(path::Type::SHADERS) + filename); }

std::vector<uint8_t> ReadShaderBinary(const std::string &filename) {
    return read_binary_file(path::Get(path::Type::SHADERS) + filename, 0);
}

std::vector<uint8_t> ReadTemp(const std::string &filename, const uint32_t count) {
    return read_binary_file(path::Get(path::Type::TEMP) + filename, count);
}

void WriteTemp(const std::vector<uint8_t> &data, const std::string &filename, uint32_t count) {
    WriteBinaryFile(data, path::Get(path::Type::TEMP) + filename, count);
}

void WriteImage(const uint8_t *data,
                const std::string &filename,
                uint32_t width,
                uint32_t height,
                uint32_t components,
                uint32_t row_stride) {
    stbi_write_png((path::Get(path::Type::SCREENSHOTS) + filename + ".png").c_str(), width, height, components, data,
                   row_stride);
}

bool WriteJson(nlohmann::json &data, const std::string &filename) {
    std::stringstream json;

    try {
        // Whitespace needed as last character is overwritten on android causing the json to be corrupt
        json << data << " ";
    } catch (std::exception &e) {
        // JSON dump errors
        LOGE(e.what())
        return false;
    }

    if (!nlohmann::json::accept(json.str())) {
        LOGE("Invalid JSON string")
        return false;
    }

    std::ofstream out_stream;
    out_stream.open(fs::path::Get(vox::fs::path::Type::GRAPHS) + filename, std::ios::out | std::ios::trunc);

    if (out_stream.good()) {
        out_stream << json.str();
    } else {
        LOGE("Could not load JSON file " + filename)
        return false;
    }

    out_stream.close();
    return true;
}

// MARK: - Extension
std::string MakeWindowsStyle(const std::string &path) {
    std::string result;
    result.resize(path.size());

    for (size_t i = 0; i < path.size(); ++i) result[i] = path[i] == '/' ? '\\' : path[i];

    return result;
}

std::string MakeNonWindowsStyle(const std::string &path) {
    std::string result;
    result.resize(path.size());

    for (size_t i = 0; i < path.size(); ++i) result[i] = path[i] == '\\' ? '/' : path[i];

    return result;
}

std::string ExtraContainingFolder(const std::string &path) {
    std::string result;

    bool extraction = false;

    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        if (extraction) result += *it;

        if (!extraction && it != path.rbegin() && (*it == '\\' || *it == '/')) extraction = true;
    }

    std::reverse(result.begin(), result.end());

    if (!result.empty() && result.back() != '\\') result += '\\';

    return result;
}

std::string ExtraElementName(const std::string &p) {
    std::string result;

    std::string path = p;
    if (!path.empty() && path.back() == '\\') path.pop_back();

    for (auto it = path.rbegin(); it != path.rend() && *it != '\\' && *it != '/'; ++it) result += *it;

    std::reverse(result.begin(), result.end());

    return result;
}

std::string ExtraExtension(const std::string &uri) {
    auto dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error{"Uri has no extension"};
    }

    return uri.substr(dot_pos + 1);
}

std::string FileTypeToString(FileType file_type) {
    switch (file_type) {
        case FileType::MODEL:
            return "Model";
        case FileType::TEXTURE:
            return "Texture";
        case FileType::SHADER:
            return "Shader";
        case FileType::MATERIAL:
            return "Material";
        case FileType::SOUND:
            return "Sound";
        case FileType::SCENE:
            return "Scene";
        case FileType::SCRIPT:
            return "Script";
        case FileType::FONT:
            return "Font";
        default:
            return "Unknown";
    }
}

FileType ExtraFileType(const std::string &path) {
    std::string ext = ExtraExtension(path);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "fbx" || ext == "obj" || ext == "gltf")
        return FileType::MODEL;
    else if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "ktx")
        return FileType::TEXTURE;
    else if (ext == "glsl" || ext == "comp" || ext == "vert" || ext == "frag")
        return FileType::SHADER;
    else if (ext == "mat")
        return FileType::MATERIAL;
    else if (ext == "wav" || ext == "mp3" || ext == "ogg")
        return FileType::SOUND;
    else if (ext == "scene")
        return FileType::SCENE;
    else if (ext == "lua")
        return FileType::SCRIPT;
    else if (ext == "ttf")
        return FileType::FONT;

    return FileType::UNKNOWN;
}

}  // namespace vox::fs
