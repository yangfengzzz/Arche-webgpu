//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <json/json.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "vox.animation/offline/tools/import2vox.h"
#include "vox.animation/offline/tools/import2vox_anim.h"
#include "vox.animation/offline/tools/import2vox_config.h"
#include "vox.animation/offline/tools/import2vox_skel.h"
#include "vox.base/io/stream.h"
#include "vox.base/logging.h"
#include "vox/options/options.h"

// Declares command line options.
VOX_OPTIONS_DECLARE_STRING(file, "Specifies input file", "", true)

static bool ValidateEndianness(const vox::options::Option& _option, int /*_argc*/) {
    const vox::options::StringOption& option = static_cast<const vox::options::StringOption&>(_option);
    bool valid = std::strcmp(option.value(), "native") == 0 || std::strcmp(option.value(), "little") == 0 ||
                 std::strcmp(option.value(), "big") == 0;
    if (!valid) {
        vox::log::Err() << "Invalid endianness option \"" << option << "\"" << std::endl;
    }
    return valid;
}

VOX_OPTIONS_DECLARE_STRING_FN(endian,
                              "Selects output endianness mode. Can be \"native\" (same as current "
                              "platform), \"little\" or \"big\".",
                              "native",
                              false,
                              &ValidateEndianness)

vox::Endianness InitializeEndianness() {
    // Initializes output endianness from options.
    vox::Endianness endianness = vox::GetNativeEndianness();
    if (std::strcmp(OPTIONS_endian, "little") == 0) {
        endianness = vox::kLittleEndian;
    } else if (std::strcmp(OPTIONS_endian, "big") == 0) {
        endianness = vox::kBigEndian;
    }
    vox::log::LogV() << (endianness == vox::kLittleEndian ? "Little" : "Big")
                     << " endian output binary format selected." << std::endl;
    return endianness;
}

static bool ValidateLogLevel(const vox::options::Option& _option, int /*_argc*/) {
    const vox::options::StringOption& option = static_cast<const vox::options::StringOption&>(_option);
    bool valid = std::strcmp(option.value(), "verbose") == 0 || std::strcmp(option.value(), "standard") == 0 ||
                 std::strcmp(option.value(), "silent") == 0;
    if (!valid) {
        vox::log::Err() << "Invalid log level option \"" << option << "\"" << std::endl;
    }
    return valid;
}

VOX_OPTIONS_DECLARE_STRING_FN(log_level,
                              "Selects log level. Can be \"silent\", \"standard\" or \"verbose\".",
                              "standard",
                              false,
                              &ValidateLogLevel)

void InitializeLogLevel() {
    vox::log::Level log_level = vox::log::GetLevel();
    if (std::strcmp(OPTIONS_log_level, "silent") == 0) {
        log_level = vox::log::kSilent;
    } else if (std::strcmp(OPTIONS_log_level, "standard") == 0) {
        log_level = vox::log::kStandard;
    } else if (std::strcmp(OPTIONS_log_level, "verbose") == 0) {
        log_level = vox::log::kVerbose;
    }
    vox::log::SetLevel(log_level);
    vox::log::LogV() << "Verbose log level activated." << std::endl;
}

namespace vox {
namespace animation {
namespace offline {

int OzzImporter::operator()(int _argc, const char** _argv) {
    // Parses arguments.
    vox::options::ParseResult parse_result =
            vox::options::ParseCommandLine(_argc, _argv, "2.0",
                                           "Imports skeleton and animations from a file and converts it to vox "
                                           "binary raw or runtime data format.");
    if (parse_result != vox::options::kSuccess) {
        return parse_result == vox::options::kExitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    // Initialize general executable options.
    InitializeLogLevel();
    const vox::Endianness endianness = InitializeEndianness();

    Json::Value config;
    if (!ProcessConfiguration(&config)) {
        // Specific error message are reported during configuration processing.
        return EXIT_FAILURE;
    }

    // Ensures file to import actually exist.
    if (!vox::io::File::Exist(OPTIONS_file)) {
        vox::log::Err() << "File \"" << OPTIONS_file << "\" doesn't exist." << std::endl;
        return EXIT_FAILURE;
    }

    // Imports animations from the document.
    vox::log::Log() << "Importing file \"" << OPTIONS_file << "\"" << std::endl;
    if (!Load(OPTIONS_file)) {
        vox::log::Err() << "Failed to import file \"" << OPTIONS_file << "\"." << std::endl;
        return EXIT_FAILURE;
    }

    // Handles skeleton import processing
    if (!ImportSkeleton(config, this, endianness)) {
        return EXIT_FAILURE;
    }

    // Handles animations import processing
    if (!ImportAnimations(config, this, endianness)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

vox::string OzzImporter::BuildFilename(const char* _filename, const char* _data_name) const {
    // Fixup invalid characters for a path.
    vox::string data_name(_data_name);
    for (const char c : {'<', '>', ':', '/', '\\', '|', '?', '*'}) {
        std::replace(data_name.begin(), data_name.end(), c, '_');
    }

    // Replaces asterisk with data_name
    bool used = false;
    vox::string output(_filename);
    for (size_t asterisk = output.find('*'); asterisk != std::string::npos; used = true, asterisk = output.find('*')) {
        output.replace(asterisk, 1, data_name);
    }

    // Displays a log only if data name was renamed and used as a filename.
    if (used && data_name != _data_name) {
        vox::log::Log() << "Resource name \"" << _data_name << "\" was changed to \"" << data_name
                        << "\" in order to be used as a valid filename." << std::endl;
    }
    return output;
}
}  // namespace offline
}  // namespace animation
}  // namespace vox
