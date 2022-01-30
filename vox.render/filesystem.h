//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef filesystem_hpp
#define filesystem_hpp

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include <json.hpp>

namespace vox {
namespace fs {
namespace path {
enum Type {
    //Relative paths
    Assets,
    Shaders,
    Storage,
    Screenshots,
    Logs,
    Graphs,
    /* NewFolder */
    TotalRelativePathTypes,
    
    //Special paths
    ExternalStorage,
    WorkingDir = ExternalStorage,
    Temp
};

extern const std::unordered_map<Type, std::string> relative_paths;

/**
 * @brief Gets the absolute path of a given type or a specific file
 * @param type The type of file path
 * @param file (Optional) The filename
 * @throws runtime_error if the platform didn't initialize each path properly, path wasn't found or the path was found but is empty
 * @return Path to the directory of a certain type
 */
const std::string get(const Type type, const std::string &file = "");
}        // namespace path

/**
 * @brief Helper to tell if a given path is a directory
 * @param path A path to a directory
 * @return True if the path points to a valid directory, false if not
 */
bool isDirectory(const std::string &path);

/**
 * @brief Checks if a file exists
 * @param filename The filename to check
 * @return True if the path points to a valid file, false if not
 */
bool isFile(const std::string &filename);

/**
 * @brief Platform specific implementation to create a directory
 * @param path A path to a directory
 */
void createDirectory(const std::string &path);

/**
 * @brief Recursively creates a directory
 * @param root The root directory that the path is relative to
 * @param path A path in the format 'this/is/an/example/path/'
 */
void createPath(const std::string &root, const std::string &path);

/**
 * @brief Helper to read an asset file into a byte-array
 *
 * @param filename The path to the file (relative to the assets directory)
 * @param count (optional) How many bytes to read. If 0 or not specified, the size
 * of the file will be used.
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> readAsset(const std::string &filename, const uint32_t count = 0);

/**
 * @brief Helper to read a shader file into a single string
 *
 * @param filename The path to the file (relative to the assets directory)
 * @return A string of the text in the shader file
 */
std::string readShader(const std::string &filename);

/**
 * @brief Helper to read a shader file into a byte-array
 *
 * @param filename The path to the file (relative to the assets directory)
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> readShaderBinary(const std::string &filename);

/**
 * @brief Helper to read a temporary file into a byte-array
 *
 * @param filename The path to the file (relative to the temporary storage directory)
 * @param count (optional) How many bytes to read. If 0 or not specified, the size
 * of the file will be used.
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> readTemp(const std::string &filename, const uint32_t count = 0);

/**
 * @brief Helper to write to a file in temporary storage
 *
 * @param data A vector filled with data to write
 * @param filename The path to the file (relative to the temporary storage directory)
 * @param count (optional) How many bytes to write. If 0 or not specified, the size
 * of data will be used.
 */
void writeTemp(const std::vector<uint8_t> &data, const std::string &filename, const uint32_t count = 0);

/**
 * @brief Helper to write to a png image in permanent storage
 *
 * @param data       A vector filled with pixel data to write in (R, G, B, A) format
 * @param filename   The name of the image file without an extension
 * @param width      The width of the image
 * @param height     The height of the image
 * @param components The number of bytes per element
 * @param row_stride The stride in bytes of a row of pixels
 */
void writeImage(const uint8_t *data, const std::string &filename,
                const uint32_t width, const uint32_t height,
                const uint32_t components, const uint32_t row_stride);

/**
 * @brief Helper to output a json graph
 *
 * @param data A json object
 * @param filename The name of the file
 */
bool writeJson(nlohmann::json &data, const std::string &filename);

/**
 * @brief Extracts the extension from an uri
 * @param uri An uniform Resource Identifier
 * @return The extension
 */
std::string extraExtension(const std::string &uri);

}        // namespace fs
}        // namespace vox

#endif /* filesystem_hpp */
