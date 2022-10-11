//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz_skel.h"

#include <json/json.h>

#include <cstdlib>
#include <cstring>
#include <iomanip>

#include "asset_pipeline/importer/import2ozz.h"
#include "asset_pipeline/importer/import2ozz_config.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.animation/offline/skeleton_builder.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.base/containers/map.h"
#include "vox.base/containers/set.h"
#include "vox.base/io/archive.h"
#include "vox.base/io/stream.h"
#include "vox.base/logging.h"
#include "vox.base/memory/unique_ptr.h"

namespace vox::animation::offline {
namespace {

// Uses a set to detect names uniqueness.
typedef vox::set<const char*, vox::str_less> Names;

bool ValidateJointNamesUniquenessRecurse(const RawSkeleton::Joint::Children& _joints, Names* _names) {
    for (size_t i = 0; i < _joints.size(); ++i) {
        const RawSkeleton::Joint& joint = _joints[i];
        const char* name = joint.name.c_str();
        if (!_names->insert(name).second) {
            vox::log::Err() << "Skeleton contains at least one non-unique joint name \"" << name
                            << "\", which is not supported." << std::endl;
            return false;
        }
        if (!ValidateJointNamesUniquenessRecurse(_joints[i].children, _names)) {
            return false;
        }
    }
    return true;
}

bool ValidateJointNamesUniqueness(const RawSkeleton& _skeleton) {
    Names joint_names;
    return ValidateJointNamesUniquenessRecurse(_skeleton.roots, &joint_names);
}

void LogHierarchy(const RawSkeleton::Joint::Children& _children, int _depth = 0) {
    const std::streamsize pres = vox::log::LogV().stream().precision();
    for (size_t i = 0; i < _children.size(); ++i) {
        const RawSkeleton::Joint& joint = _children[i];
        vox::log::LogV() << std::setw(_depth) << std::setfill('.') << "";
        vox::log::LogV() << joint.name.c_str() << std::setprecision(4) << " t: " << joint.transform.translation.x
                         << ", " << joint.transform.translation.y << ", " << joint.transform.translation.z
                         << " r: " << joint.transform.rotation.x << ", " << joint.transform.rotation.y << ", "
                         << joint.transform.rotation.z << ", " << joint.transform.rotation.w
                         << " s: " << joint.transform.scale.x << ", " << joint.transform.scale.y << ", "
                         << joint.transform.scale.z << std::endl;

        // Recurse
        LogHierarchy(joint.children, _depth + 1);
    }
    vox::log::LogV() << std::setprecision(static_cast<int>(pres));
}
}  // namespace

bool ImportSkeleton(const Json::Value& _config, OzzImporter* _importer, const vox::Endianness _endianness) {
    const Json::Value& skeleton_config = _config["skeleton"];
    const Json::Value& import_config = skeleton_config["import"];

    // First check that we're actually expecting to import a skeleton.
    if (!import_config["enable"].asBool()) {
        vox::log::Log() << "Skeleton build disabled, import will be skipped." << std::endl;
        return true;
    }

    // Setup node types import properties.
    const Json::Value& types_config = import_config["types"];
    OzzImporter::NodeType types = {};
    types.skeleton = types_config["skeleton"].asBool();
    types.marker = types_config["marker"].asBool();
    types.camera = types_config["camera"].asBool();
    types.geometry = types_config["geometry"].asBool();
    types.light = types_config["light"].asBool();
    types.null = types_config["null"].asBool();
    types.any = types_config["any"].asBool();

    RawSkeleton raw_skeleton;
    if (!_importer->Import(&raw_skeleton, types)) {
        vox::log::Err() << "Failed to import skeleton." << std::endl;
        return false;
    }

    // Log skeleton hierarchy
    if (vox::log::GetLevel() == vox::log::kVerbose) {
        LogHierarchy(raw_skeleton.roots);
    }

    // Non unique joint names are not supported.
    if (!(ValidateJointNamesUniqueness(raw_skeleton))) {
        // Log Err is done by the validation function.
        return false;
    }

    // Needs to be done before opening the output file, so that if it fails then
    // there's no invalid file outputted.
    unique_ptr<Skeleton> skeleton;
    if (!import_config["raw"].asBool()) {
        // Builds runtime skeleton.
        vox::log::Log() << "Builds runtime skeleton." << std::endl;
        SkeletonBuilder builder;
        skeleton = builder(raw_skeleton);
        if (!skeleton) {
            vox::log::Err() << "Failed to build runtime skeleton." << std::endl;
            return false;
        }
    }

    // Prepares output stream. File is a RAII so it will close automatically at
    // the end of this scope.
    // Once the file is opened, nothing should fail as it would leave an invalid
    // file on the disk.
    {
        const char* filename = skeleton_config["filename"].asCString();
        vox::log::Log() << "Opens output file: " << filename << std::endl;
        vox::io::File file(filename, "wb");
        if (!file.opened()) {
            vox::log::Err() << "Failed to open output file: \"" << filename << "\"." << std::endl;
            return false;
        }

        // Initializes output archive.
        vox::io::OArchive archive(&file, _endianness);

        // Fills output archive with the skeleton.
        if (import_config["raw"].asBool()) {
            vox::log::Log() << "Outputs RawSkeleton to binary archive." << std::endl;
            archive << raw_skeleton;
        } else {
            vox::log::Log() << "Outputs Skeleton to binary archive." << std::endl;
            archive << *skeleton;
        }
        vox::log::Log() << "Skeleton binary archive successfully outputted." << std::endl;
    }

    return true;
}
}  // namespace vox::animation::offline
