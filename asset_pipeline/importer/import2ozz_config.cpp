//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz_config.h"

#include <json/json.h>

#include <cstring>
#include <fstream>
#include <sstream>

#include "asset_pipeline/importer/import2ozz.h"
#include "asset_pipeline/importer/import2ozz_anim.h"
#include "asset_pipeline/importer/import2ozz_track.h"
#include "asset_pipeline/options.h"
#include "vox.animation/offline/animation_optimizer.h"
#include "vox.animation/offline/track_optimizer.h"
#include "vox.base/logging.h"

bool ValidateExclusiveConfigOption(const vox::options::Option& _option, int _argc);
VOX_OPTIONS_DECLARE_STRING_FN(
        config, "Specifies input configuration string in json format", "", false, &ValidateExclusiveConfigOption)
VOX_OPTIONS_DECLARE_STRING_FN(
        config_file, "Specifies input configuration file in json format", "", false, &ValidateExclusiveConfigOption)

// Validate exclusive config options.
bool ValidateExclusiveConfigOption(const vox::options::Option& _option, int _argc) {
    (void)_option;
    (void)_argc;
    bool not_exclusive = OPTIONS_config_file.value()[0] != 0 && OPTIONS_config.value()[0] != 0;
    if (not_exclusive) {
        LOGE("--config and --config_file are exclusive options.")
    }
    return !not_exclusive;
}

VOX_OPTIONS_DECLARE_STRING(config_dump_reference, "Dumps reference json configuration to specified file.", "", false)

namespace vox::animation::offline {
namespace {

template <typename Type>
struct ToJsonType;

template <>
struct ToJsonType<int> {
    static Json::ValueType type() { return Json::intValue; }
};
template <>
struct ToJsonType<unsigned int> {
    static Json::ValueType type() { return Json::uintValue; }
};
template <>
struct ToJsonType<float> {
    static Json::ValueType type() { return Json::realValue; }
};
template <>
struct ToJsonType<const char*> {
    static Json::ValueType type() { return Json::stringValue; }
};
template <>
struct ToJsonType<bool> {
    static Json::ValueType type() { return Json::booleanValue; }
};

const char* JsonTypeToString(Json::ValueType _type) {
    switch (_type) {
        case Json::nullValue:
            return "null";
        case Json::intValue:
            return "integer";
        case Json::uintValue:
            return "unsigned integer";
        case Json::realValue:
            return "float";
        case Json::stringValue:
            return "UTF-8 string";
        case Json::booleanValue:
            return "boolean";
        case Json::arrayValue:
            return "array";
        case Json::objectValue:
            return "object";
        default:
            assert(false && "unknown json type");
            return "unknown";
    }
}

bool IsCompatibleType(Json::ValueType _type, Json::ValueType _expected) {
    switch (_expected) {
        case Json::nullValue:
            return _type == Json::nullValue;
        case Json::intValue:
            return _type == Json::intValue || _type == Json::uintValue;
        case Json::uintValue:
            return _type == Json::uintValue;
        case Json::realValue:
            return _type == Json::realValue || _type == Json::intValue || _type == Json::uintValue;
        case Json::stringValue:
            return _type == Json::stringValue;
        case Json::booleanValue:
            return _type == Json::booleanValue;
        case Json::arrayValue:
            return _type == Json::arrayValue;
        case Json::objectValue:
            return _type == Json::objectValue;
        default:
            assert(false && "unknown json type");
            return false;
    }
}

bool MakeDefaultArray(Json::Value& _parent, const char* _name, const char* _comment, bool _empty) {
    // Check if exists first.
    const bool existed = _parent.isMember(_name);
    // Create in any case
    Json::Value* member = &_parent[_name];
    if (!existed) {
        member->resize(_empty ? 0 : 1);
        assert(member->isArray());
    }

    // Pushes comment if there's not one already.
    if (*_comment != 0 && !member->hasComment(Json::commentBefore)) {
        member->setComment(std::string("//  ") + _comment, Json::commentBefore);
    }

    return existed;
}

bool MakeDefaultObject(Json::Value& _parent, const char* _name, const char* _comment) {
    // Check if exists first.
    const bool existed = _parent.isMember(_name);
    // Create in any case
    Json::Value* member = &_parent[_name];
    if (!existed) {
        *member = Json::Value(Json::objectValue);
        assert(member->isObject());
    }

    // Pushes comment if there's not one already.
    if (*_comment != 0 && !member->hasComment(Json::commentBefore)) {
        member->setComment(std::string("//  ") + _comment, Json::commentBefore);
    }

    return existed;
}

template <typename Type>
bool MakeDefault(Json::Value& _parent, const char* _name, Type _value, const char* _comment) {
    // Check if exists first.
    const bool existed = _parent.isMember(_name);
    // Create in any case
    Json::Value* member = &_parent[_name];
    if (!existed) {
        *member = _value;
        assert(IsCompatibleType(member->type(), ToJsonType<Type>::type()));
    }

    // Pushes comment if there's not one already.
    if (*_comment != 0 && !member->hasComment(Json::commentAfterOnSameLine)) {
        member->setComment(std::string("//  ") + _comment, Json::commentAfterOnSameLine);
    }

    return existed;
}

bool SanitizeSkeletonJointTypes(Json::Value& _root, bool _all_options) {
    (void)_all_options;
    MakeDefault(_root, "skeleton", true, "Uses skeleton nodes as skeleton joints.");
    MakeDefault(_root, "marker", false, "Uses marker nodes as skeleton joints.");
    MakeDefault(_root, "camera", false, "Uses camera nodes as skeleton joints.");
    MakeDefault(_root, "geometry", false, "Uses geometry nodes as skeleton joints.");
    MakeDefault(_root, "light", false, "Uses light nodes as skeleton joints.");
    MakeDefault(_root, "null", false, "Uses null nodes as skeleton joints.");
    MakeDefault(_root, "any", false,
                "Uses any node type as skeleton joints, including those listed "
                "above and any other.");
    return true;
}

bool SanitizeSkeletonImport(Json::Value& _root, bool _all_options) {
    (void)_all_options;
    MakeDefault(_root, "enable", true, "Imports (from source data file) and writes skeleton output file.");
    MakeDefault(_root, "raw", false, "Outputs raw skeleton.");
    MakeDefaultObject(_root, "types", "Define nodes types that should be considered as skeleton joints.");
    SanitizeSkeletonJointTypes(_root["types"], _all_options);
    return true;
}

bool SanitizeSkeleton(Json::Value& _root, bool _all_options) {
    MakeDefault(_root, "filename", "skeleton.vox",
                "Specifies skeleton input/output filename. The file will be "
                "outputted if import is true. It will also be used as an input "
                "reference during animations import.");
    MakeDefaultObject(_root, "import", "Define skeleton import settings.");
    SanitizeSkeletonImport(_root["import"], _all_options);
    return true;
}

bool SanitizeOptimizationSetting(Json::Value& _root) {
    const AnimationOptimizer::Setting default_setting;
    MakeDefault(_root, "tolerance", default_setting.tolerance,
                "The maximum error that an optimization is allowed to generate "
                "on a whole joint hierarchy.");
    MakeDefault(_root, "distance", default_setting.distance,
                "The distance (from the joint) at which error is measured. This "
                "allows to emulate effect on skinning.");
    return true;
}

bool SanitizeJointsSetting(Json::Value& _root) {
    MakeDefault(_root, "name", "*", "Joint name. Wildcard characters \'*\' and \'?\' are supported");
    SanitizeOptimizationSetting(_root);
    return true;
}

bool SanitizeOptimizationSettings(Json::Value& _root, bool _all_options) {
    SanitizeOptimizationSetting(_root);

    MakeDefaultArray(_root, "override", "Per joint optimization setting override", !_all_options);
    Json::Value& joints = _root["override"];
    for (auto& joint : joints) {
        if (!SanitizeJointsSetting(joint)) {
            return false;
        }
    }

    return true;
}

bool SanitizeTrackImport(Json::Value& _root) {
    MakeDefault(_root, "filename", "*.vox",
                "Specifies track output filename(s). Use a \'*\' character "
                "to specify part(s) of the filename that should be replaced by "
                "the track (aka \"joint_name-property_name\") name.");
    MakeDefault(_root, "joint_name", "*",
                "Name of the joint that contains the property to import. "
                "Wildcard characters '*' and '?' are supported.");
    MakeDefault(_root, "property_name", "*",
                "Name of the property to import. Wildcard characters '*' and '?' "
                "are supported.");
    MakeDefault(_root, "type", "float1",
                "Type of the property, can be float1 to float4, point and vector "
                "(aka float3 with scene unit and axis conversion).");
    const char* type_name = _root["type"].asCString();
    if (!PropertyTypeConfig::IsValidEnumName(type_name)) {
        LOGE("Invalid value {} for import track type property. Type can be float1 "
             "to float4, point and vector (aka float3 with scene "
             "unit and axis conversion).",
             type_name)
        return false;
    }

    MakeDefault(_root, "raw", false, "Outputs raw track.");
    MakeDefault(_root, "optimize", true, "Activates keyframes optimization.");
    MakeDefault(_root, "optimization_tolerance", TrackOptimizer().tolerance, "Optimization tolerance");

    return true;
}
/*
bool SanitizeTrackMotion(Json::Value& _root) {
  MakeDefault(_root, "joint_name", "",
              "Name of the joint that contains the property to import. "
              "Wildcard characters '*' and '?' are supported.");
  MakeDefault(_root, "output", "*.vox",
              "Specifies track output file(s). Use a \'*\' character to "
              "specify part(s) of the filename that should be replaced by the "
              "joint_name.");
  MakeDefault(_root, "optimization_tolerance",
              TrackOptimizer().tolerance,
              "Optimization tolerance");
  return true;
}*/

bool SanitizeTrack(Json::Value& _root, bool _all_options) {
    MakeDefaultArray(_root, "properties", "Properties to import.", !_all_options);
    Json::Value& imports = _root["properties"];
    for (auto& import : imports) {
        if (!SanitizeTrackImport(import)) {
            return false;
        }
    }
    /*
      MakeDefaultArray(_root, "motions", "Motions tracks to generate.",
                       !_all_options);
      Json::Value& motions = _root["motions"];
      for (Json::ArrayIndex i = 0; i < motions.size(); ++i) {
        if (!SanitizeTrackMotion(motions[i])) {
          return false;
        }
      }*/
    return true;
}

bool SanitizeAnimation(Json::Value& _root, bool _all_options) {
    MakeDefault(_root, "clip", "*",
                "Specifies clip name (take) of the animation to import from the "
                "source file. Wildcard characters \'*\' and \'?\' are supported");

    MakeDefault(_root, "filename", "*.vox",
                "Specifies animation output filename. Use a \'*\' character to "
                "specify part(s) of the filename that should be replaced by the "
                "clip name.");

    MakeDefault(_root, "raw", false, "Outputs raw animation.");

    MakeDefault(_root, "additive", false, "Creates a delta animation that can be used for additive blending.");
    MakeDefault(_root, "additive_reference", "animation",
                "Select reference pose to use to build additive/delta animation. "
                "Can be \"animation\" to use the 1st animation keyframe as "
                "reference, or \"skeleton\" to use skeleton rest pose.");

    if (!AdditiveReference::IsValidEnumName(_root["additive_reference"].asCString())) {
        LOGE("Invalid additive reference pose {} Can be \"animation\" to use the 1st animation keyframe "
             "as reference, or \"skeleton\" to use skeleton rest "
             "pose.",
             _root["additive_reference"].asCString())
        return false;
    }

    MakeDefault(_root, "sampling_rate", 0.f,
                "Selects animation sampling rate in hertz. Set a value <= 0 to "
                "use imported scene default frame rate.");

    MakeDefault(_root, "optimize", true, "Activates keyframes reduction optimization.");

    SanitizeOptimizationSettings(_root["optimization_settings"], _all_options);

    MakeDefaultArray(_root, "tracks", "Tracks to build.", !_all_options);
    Json::Value& tracks = _root["tracks"];
    for (auto& track : tracks) {
        if (!SanitizeTrack(track, _all_options)) {
            return false;
        }
    }

    return true;
}  // namespace

bool SanitizeRoot(Json::Value& _root, bool _all_options) {
    // Skeleton
    MakeDefaultObject(_root, "skeleton", "Skeleton to import");
    SanitizeSkeleton(_root["skeleton"], _all_options);

    // Animations.
    // Forces array creation as it's expected for the defaultconfiguration.
    MakeDefaultArray(_root, "animations", "Animations to import.", false);
    Json::Value& animations = _root["animations"];
    for (auto& animation : animations) {
        if (!SanitizeAnimation(animation, _all_options)) {
            return false;
        }
    }

    return true;
}

bool RecursiveCheck(const Json::Value& _root, const Json::Value& _expected, const vox::string& _name) {
    if (!IsCompatibleType(_root.type(), _expected.type())) {
        // It's a failure to have a wrong member type.
        LOGE("Invalid type {} for json member {}. {} expected.", JsonTypeToString(_root.type()), _name,
             JsonTypeToString(_expected.type()))
        return false;
    }

    if (_root.isArray()) {
        assert(_expected.isArray());
        for (Json::ArrayIndex i = 0; i < _root.size(); ++i) {
            std::ostringstream istr;
            istr << "[" << i << "]";
            if (!RecursiveCheck(_root[i], _expected[0], _name + istr.str().c_str())) {
                return false;
            }
        }
    } else if (_root.isObject()) {
        assert(_expected.isObject());
        for (auto it = _root.begin(); it != _root.end(); it++) {
            const std::string& name = it.name();
            if (!_expected.isMember(name)) {
                LOGE("Invalid json member {}.{}", _name, name.c_str())
                return false;
            }
            const Json::Value& expected_member = _expected[name];
            if (!RecursiveCheck(*it, expected_member, _name + "." + name.c_str())) {
                return false;
            }
        }
    }
    return true;
}

std::string ToString(const Json::Value& _value) {
    // Format configuration
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    builder["precision"] = 4;
    return Json::writeString(builder, _value);
}

bool DumpConfig(const char* _path, const Json::Value& _config) {
    if (_path[0] != 0) {
        LOGI("Opens config file to dump: {}", _path)
        std::ofstream file(_path);
        if (!file.is_open()) {
            LOGE("Failed to open config file to dump: {}", _path)
            return false;
        }
        const std::string& document = ToString(_config);
        file << document;
    }
    return true;
}
}  // namespace

bool CompareName(const char* _a, const char* _b) { return std::strcmp(_a, _b) == 0; }

bool ProcessConfiguration(Json::Value* _config) {
    if (!_config) {
        return false;
    }

    // Use {} as a default config, otherwise take the one specified as argument.
    std::string config_string = "{}";
    // Takes config from program options.
    if (OPTIONS_config.value()[0] != 0) {
        config_string = OPTIONS_config.value();
    } else if (OPTIONS_config_file.value()[0] != 0) {
        LOGI("Opens config file: {}.", OPTIONS_config_file)

        std::ifstream file(OPTIONS_config_file.value());
        if (!file.is_open()) {
            LOGE("Failed to open config file: {}.", OPTIONS_config_file)
            return false;
        }
        config_string.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    } else {
        LOGI("No configuration provided, using default configuration.")
    }

    Json::Reader json_builder;
    if (!json_builder.parse(config_string, *_config, true)) {
        LOGE("Error while parsing configuration string: {}", json_builder.getFormattedErrorMessages())
        return false;
    }

    // Build the reference config to compare it with provided one and detect
    // unexpected members.
    Json::Value ref_config;
    if (!SanitizeRoot(ref_config, true)) {
        assert(false && "Failed to sanitized default configuration.");
    }

    // All format errors are reported within that function
    if (!RecursiveCheck(*_config, ref_config, "root")) {
        return false;
    }

    // Sanitized provided config.
    if (!SanitizeRoot(*_config, false)) {
        return false;
    }

    // Dumps reference config to file.
    if (!DumpConfig(OPTIONS_config_dump_reference.value(), ref_config)) {
        return false;
    }

    return true;
}
}  // namespace vox::animation::offline
