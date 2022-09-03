//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sys/stat.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace vox::fs {
namespace path {
enum Type {
    // Relative paths
    ASSETS,
    SHADERS,
    STORAGE,
    SCREENSHOTS,
    LOGS,
    GRAPHS,
    /* NewFolder */
    TOTAL_RELATIVE_PATH_TYPES,

    // Special paths
    EXTERNAL_STORAGE,
    WORKING_DIR = EXTERNAL_STORAGE,
    TEMP
};

extern const std::unordered_map<Type, std::string> kRelativePaths;

/**
 * @brief Gets the absolute path of a given type or a specific file
 * @param type The type of file path
 * @param file (Optional) The filename
 * @throws runtime_error if the platform didn't initialize each path properly, path wasn't found or the path was found
 * but is empty
 * @return Path to the directory of a certain type
 */
std::string Get(Type type, const std::string &file = "");
}  // namespace path

/**
 * @brief Helper to tell if a given path is a directory
 * @param path A path to a directory
 * @return True if the path points to a valid directory, false if not
 */
bool IsDirectory(const std::string &path);

/**
 * @brief Checks if a file exists
 * @param filename The filename to check
 * @return True if the path points to a valid file, false if not
 */
bool IsFile(const std::string &filename);

/**
 * @brief Platform specific implementation to create a directory
 * @param path A path to a directory
 */
void CreateDirectory(const std::string &path);

/**
 * @brief Recursively creates a directory
 * @param root The root directory that the path is relative to
 * @param path A path in the format 'this/is/an/example/path/'
 */
void CreatePath(const std::string &root, const std::string &path);

/**
 * @brief Helper to read an asset file into a byte-array
 *
 * @param filename The path to the file (relative to the assets directory)
 * @param count (optional) How many bytes to read. If 0 or not specified, the size
 * of the file will be used.
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> ReadAsset(const std::string &filename, uint32_t count = 0);

/**
 * @brief Helper to read a shader file into a single string
 *
 * @param filename The path to the file (relative to the assets directory)
 * @return A string of the text in the shader file
 */
std::string ReadShader(const std::string &filename);

/**
 * @brief Helper to read a shader file into a byte-array
 *
 * @param filename The path to the file (relative to the assets directory)
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> ReadShaderBinary(const std::string &filename);

/**
 * @brief Helper to read a temporary file into a byte-array
 *
 * @param filename The path to the file (relative to the temporary storage directory)
 * @param count (optional) How many bytes to read. If 0 or not specified, the size
 * of the file will be used.
 * @return A vector filled with data read from the file
 */
std::vector<uint8_t> ReadTemp(const std::string &filename, uint32_t count = 0);

/**
 * @brief Helper to write to a file in temporary storage
 *
 * @param data A vector filled with data to write
 * @param filename The path to the file (relative to the temporary storage directory)
 * @param count (optional) How many bytes to write. If 0 or not specified, the size
 * of data will be used.
 */
void WriteTemp(const std::vector<uint8_t> &data, const std::string &filename, uint32_t count = 0);

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
void WriteImage(const uint8_t *data,
                const std::string &filename,
                uint32_t width,
                uint32_t height,
                uint32_t components,
                uint32_t row_stride);

/**
 * @brief Helper to output a json graph
 *
 * @param data A json object
 * @param filename The name of the file
 */
bool WriteJson(nlohmann::json &data, const std::string &filename);

// MARK: - extension
enum class FileType { UNKNOWN, MODEL, TEXTURE, SHADER, MATERIAL, SOUND, SCENE, SCRIPT, FONT };

/**
 * Returns the windows style version of the given path ('/' replaced by '\')
 */
std::string MakeWindowsStyle(const std::string &path);

/**
 * Returns the non-windows style version of the given path ('\' replaced by '/')
 */
std::string MakeNonWindowsStyle(const std::string &path);

/**
 * Returns the containing folder of the file or folder identified by the given path
 */
std::string ExtraContainingFolder(const std::string &path);

/**
 * Returns the name of the file or folder identified by the given path
 */
std::string ExtraElementName(const std::string &path);

/**
 * @brief Extracts the extension from an uri
 * @param uri An uniform Resource Identifier
 * @return The extension
 */
std::string ExtraExtension(const std::string &uri);

/**
 * @brief Convert the EFileType value to a string
 */
std::string FileTypeToString(FileType file_type);

/**
 * @brief Returns the file type of the file identified by the given path
 */
FileType ExtraFileType(const std::string &path);

}  // namespace vox::fs
