//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz_track.h"

#include <json/json.h>

#include <cstdlib>
#include <cstring>

#include "asset_pipeline/importer/import2ozz.h"
#include "asset_pipeline/importer/import2ozz_config.h"
#include "asset_pipeline/options.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.animation/offline/track_builder.h"
#include "vox.animation/offline/track_optimizer.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.animation/runtime/track.h"
#include "vox.base/io/archive.h"
#include "vox.base/io/stream.h"
#include "vox.base/logging.h"
#include "vox.base/memory/unique_ptr.h"

namespace vox {
namespace animation {
namespace offline {
namespace {

template <typename _Track>
void DisplaysOptimizationstatistics(const _Track& _non_optimized, const _Track& _optimized) {
    const size_t opt = _optimized.keyframes.size();
    const size_t non_opt = _non_optimized.keyframes.size();

    // Computes optimization ratios.
    float ratio = opt != 0 ? 1.f * non_opt / opt : 0.f;

    vox::log::LogV log;
    vox::log::FloatPrecision precision_scope(log, 1);
    log << "Optimization stage results: " << ratio << ":1" << std::endl;
}

bool IsCompatiblePropertyType(OzzImporter::NodeProperty::Type _src, OzzImporter::NodeProperty::Type _dest) {
    if (_src == _dest) {
        return true;
    }
    switch (_src) {
        case OzzImporter::NodeProperty::kFloat3:
            return _dest == OzzImporter::NodeProperty::kPoint || _dest == OzzImporter::NodeProperty::kVector;
        case OzzImporter::NodeProperty::kPoint:
        case OzzImporter::NodeProperty::kVector:
            return _dest == OzzImporter::NodeProperty::kFloat3;
        default:
            return false;
    }
}

template <typename _RawTrack>
struct RawTrackToTrack;

template <>
struct RawTrackToTrack<RawFloatTrack> {
    typedef FloatTrack Track;
};
template <>
struct RawTrackToTrack<RawFloat2Track> {
    typedef Float2Track Track;
};
template <>
struct RawTrackToTrack<RawFloat3Track> {
    typedef Float3Track Track;
};
template <>
struct RawTrackToTrack<RawFloat4Track> {
    typedef Float4Track Track;
};

template <typename _RawTrack>
bool Export(OzzImporter& _importer,
            const _RawTrack& _raw_track,
            const Json::Value& _config,
            const vox::Endianness _endianness) {
    // Raw track to build and output.
    _RawTrack raw_track;

    // Optimizes track if option is enabled.
    if (_config["optimize"].asBool()) {
        vox::log::LogV() << "Optimizing track." << std::endl;
        TrackOptimizer optimizer;
        optimizer.tolerance = _config["optimization_tolerance"].asFloat();
        _RawTrack raw_optimized_track;
        if (!optimizer(_raw_track, &raw_optimized_track)) {
            vox::log::Err() << "Failed to optimize track." << std::endl;
            return false;
        }

        // Displays optimization statistics.
        DisplaysOptimizationstatistics(_raw_track, raw_optimized_track);

        // Brings data back to the raw track.
        raw_track = raw_optimized_track;
    } else {
        vox::log::LogV() << "Optimization for track \"" << _raw_track.name << "\" is disabled." << std::endl;
    }

    // Builds runtime track.
    unique_ptr<typename RawTrackToTrack<_RawTrack>::Track> track;
    if (!_config["raw"].asBool()) {
        vox::log::LogV() << "Builds runtime track." << std::endl;
        TrackBuilder builder;
        track = builder(raw_track);
        if (!track) {
            vox::log::Err() << "Failed to build runtime track." << std::endl;
            return false;
        }
    }

    {
        // Prepares output stream. Once the file is opened, nothing should fail as
        // it would leave an invalid file on the disk.

        // Builds output filename.
        const vox::string filename = _importer.BuildFilename(_config["filename"].asCString(), _raw_track.name.c_str());

        vox::log::LogV() << "Opens output file: " << filename << std::endl;
        vox::io::File file(filename.c_str(), "wb");
        if (!file.opened()) {
            vox::log::Err() << "Failed to open output file: " << filename << std::endl;
            return false;
        }

        // Initializes output archive.
        vox::io::OArchive archive(&file, _endianness);

        // Fills output archive with the track.
        if (_config["raw"].asBool()) {
            vox::log::LogV() << "Outputs RawTrack to binary archive." << std::endl;
            archive << raw_track;
        } else {
            vox::log::LogV() << "Outputs Track to binary archive." << std::endl;
            archive << *track;
        }
    }

    vox::log::LogV() << "Track binary archive successfully outputted." << std::endl;

    return true;
}

template <typename _TrackType>
bool ProcessImportTrackType(OzzImporter& _importer,
                            const char* _animation_name,
                            const char* _joint_name,
                            const OzzImporter::NodeProperty& _property,
                            const OzzImporter::NodeProperty::Type _expected_type,
                            const Json::Value& _import_config,
                            const vox::Endianness _endianness) {
    bool success = true;

    vox::log::Log() << "Extracting animation track \"" << _joint_name << ":" << _property.name.c_str()
                    << "\" from animation \"" << _animation_name << "\"." << std::endl;

    _TrackType track;
    success &= _importer.Import(_animation_name, _joint_name, _property.name.c_str(), _expected_type, 0, &track);

    if (success) {
        // Give the track a name
        track.name = _joint_name;
        track.name += '-';
        track.name += _property.name.c_str();

        success &= Export(_importer, track, _import_config, _endianness);
    } else {
        vox::log::Err() << "Failed to import track \"" << _joint_name << ":" << _property.name << "\"" << std::endl;
    }

    return success;
}

bool ProcessImportTrack(OzzImporter& _importer,
                        const char* _animation_name,
                        const Skeleton& _skeleton,
                        const Json::Value& _import_config,
                        const vox::Endianness _endianness) {
    // Early out if no name is specified
    const char* joint_name_match = _import_config["joint_name"].asCString();
    const char* ppt_name_match = _import_config["property_name"].asCString();

    // Process every joint that matches.
    bool success = true;
    bool joint_found = false;
    for (int s = 0; success && s < _skeleton.num_joints(); ++s) {
        const char* joint_name = _skeleton.joint_names()[s];
        if (!strmatch(joint_name, joint_name_match)) {
            continue;
        }
        joint_found = true;

        // Node found, need to find matching properties now.
        bool ppt_found = false;
        const OzzImporter::NodeProperties properties = _importer.GetNodeProperties(joint_name);
        for (size_t p = 0; p < properties.size(); ++p) {
            const OzzImporter::NodeProperty& property = properties[p];
            // Checks property name matches
            const char* property_name = property.name.c_str();
            vox::log::LogV() << "Inspecting property " << joint_name << ":" << property_name << "\"." << std::endl;
            if (!strmatch(property_name, ppt_name_match)) {
                continue;
            }
            // Checks property type matches
            const char* expected_type_name = _import_config["type"].asCString();
            OzzImporter::NodeProperty::Type expected_type = OzzImporter::NodeProperty::kFloat1;
            bool valid_type = PropertyTypeConfig::GetEnumFromName(expected_type_name, &expected_type);
            (void)valid_type;
            assert(valid_type && "Type should have been checked during config validation");
            bool compatible_type = IsCompatiblePropertyType(property.type, expected_type);

            if (!compatible_type) {
                vox::log::Log() << "Incompatible type \"" << expected_type_name << "\" for matching property \""
                                << joint_name << ":" << property_name << "\" of type \""
                                << PropertyTypeConfig::GetEnumName(property.type) << "\"." << std::endl;
                continue;
            }

            vox::log::LogV() << "Found matching property \"" << joint_name << ":" << property_name << "\" of type \""
                             << PropertyTypeConfig::GetEnumName(property.type) << "\"." << std::endl;

            // A property has been found.
            ppt_found = true;

            // Import property depending on its type.
            switch (property.type) {
                case OzzImporter::NodeProperty::kFloat1: {
                    success &= ProcessImportTrackType<RawFloatTrack>(_importer, _animation_name, joint_name, property,
                                                                     expected_type, _import_config, _endianness);
                    break;
                }
                case OzzImporter::NodeProperty::kFloat2: {
                    success &= ProcessImportTrackType<RawFloat2Track>(_importer, _animation_name, joint_name, property,
                                                                      expected_type, _import_config, _endianness);
                    break;
                }
                case OzzImporter::NodeProperty::kFloat3:
                case OzzImporter::NodeProperty::kPoint:
                case OzzImporter::NodeProperty::kVector: {
                    success &= ProcessImportTrackType<RawFloat3Track>(_importer, _animation_name, joint_name, property,
                                                                      expected_type, _import_config, _endianness);
                    break;
                }
                case OzzImporter::NodeProperty::kFloat4: {
                    success &= ProcessImportTrackType<RawFloat4Track>(_importer, _animation_name, joint_name, property,
                                                                      expected_type, _import_config, _endianness);
                    break;
                }
                default: {
                    assert(false && "Unknown property type.");
                    success = false;
                    break;
                }
            }
        }

        if (!ppt_found) {
            vox::log::Log() << "No property found for track import definition \"" << joint_name_match << ":"
                            << ppt_name_match << "\"." << std::endl;
        }
    }

    if (!joint_found) {
        vox::log::Log() << "No joint found for track import definition \"" << joint_name_match << "\"." << std::endl;
    }

    return success;
}

/*
bool ProcessMotionTrack(OzzImporter& _importer,
                        const char* _animation_name, const Skeleton&
_skeleton, const Json::Value& _motion) { return true;
}*/
}  // namespace

bool ProcessTracks(OzzImporter& _importer,
                   const char* _animation_name,
                   const Skeleton& _skeleton,
                   const Json::Value& _config,
                   const vox::Endianness _endianness) {
    bool success = true;

    const Json::Value& imports = _config["properties"];
    for (Json::ArrayIndex i = 0; success && i < imports.size(); ++i) {
        success &= ProcessImportTrack(_importer, _animation_name, _skeleton, imports[i], _endianness);
    }

    /*
      const Json::Value& motions = _config["motions"];
      for (Json::ArrayIndex i = 0; success && i < motions.size(); ++i) {
        success &=
            ProcessMotionTrack(_importer, _animation_name, _skeleton,
      motions[i]);
      }*/

    return success;
}

PropertyTypeConfig::EnumNames PropertyTypeConfig::GetNames() {
    static const char* kNames[] = {"float1", "float2", "float3", "float4", "point", "vector"};
    const EnumNames enum_names = {VOX_ARRAY_SIZE(kNames), kNames};
    return enum_names;
}
}  // namespace offline
}  // namespace animation
}  // namespace vox
