//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz_skel.h"

#include <json/json.h>

#include "asset_pipeline/importer/import2ozz.h"
#include "vox.animation/offline/skeleton_builder.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.base/containers/map.h"
#include "vox.base/containers/set.h"
#include "vox.base/io/archive.h"
#include "vox.base/logging.h"

namespace vox::animation::offline {
namespace {

// Uses a set to detect names uniqueness.
typedef vox::set<const char*, vox::str_less> Names;

bool ValidateJointNamesUniquenessRecurse(const RawSkeleton::Joint::Children& _joints, Names* _names) {
    for (const auto& _joint : _joints) {
        const RawSkeleton::Joint& joint = _joint;
        const char* name = joint.name.c_str();
        if (!_names->insert(name).second) {
            LOGE("Skeleton contains at least one non-unique joint name {}, which is not supported.", name)
            return false;
        }
        if (!ValidateJointNamesUniquenessRecurse(_joint.children, _names)) {
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
    for (const auto& joint : _children) {
        LOGI("t:{}, {}, {}, r: {}, {}, {}, {}, s: {}, {}, {}", joint.transform.translation.x,
             joint.transform.translation.y, joint.transform.translation.z, joint.transform.rotation.x,
             joint.transform.rotation.y, joint.transform.rotation.z, joint.transform.rotation.w,
             joint.transform.scale.x, joint.transform.scale.y, joint.transform.scale.z)

        // Recurse
        LogHierarchy(joint.children, _depth + 1);
    }
}
}  // namespace

bool ImportSkeleton(const Json::Value& _config, OzzImporter* _importer, const vox::Endianness _endianness) {
    const Json::Value& skeleton_config = _config["skeleton"];
    const Json::Value& import_config = skeleton_config["import"];

    // First check that we're actually expecting to import a skeleton.
    if (!import_config["enable"].asBool()) {
        LOGI("Skeleton build disabled, import will be skipped.")
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
        LOGE("Failed to import skeleton.")
        return false;
    }

    // Log skeleton hierarchy
    LogHierarchy(raw_skeleton.roots);

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
        LOGI("Builds runtime skeleton.")
        SkeletonBuilder builder;
        skeleton = builder(raw_skeleton);
        if (!skeleton) {
            LOGE("Failed to build runtime skeleton.")
            return false;
        }
    }

    // Prepares output stream. File is a RAII, so it will close automatically at
    // the end of this scope.
    // Once the file is opened, nothing should fail as it would leave an invalid
    // file on the disk.
    {
        const char* filename = skeleton_config["filename"].asCString();
        LOGI("Opens output file: {}", filename)
        vox::io::File file(filename, "wb");
        if (!file.opened()) {
            LOGE("Failed to open output file: {}", filename)
            return false;
        }

        // Initializes output archive.
        vox::io::OArchive archive(&file, _endianness);

        // Fills output archive with the skeleton.
        if (import_config["raw"].asBool()) {
            LOGI("Outputs RawSkeleton to binary archive.")
            archive << raw_skeleton;
        } else {
            LOGI("Outputs Skeleton to binary archive.")
            archive << *skeleton;
        }
        LOGI("Skeleton binary archive successfully outputted.")
    }

    return true;
}
}  // namespace vox::animation::offline
