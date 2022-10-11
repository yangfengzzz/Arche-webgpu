//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/raw_animation.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.animation/offline/raw_track.h"
#include "vox.animation/offline/importer/export.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/vector.h"
#include "vox.base/macros.h"

namespace vox {
namespace animation {

class Skeleton;

namespace offline {

// Defines vox asset_pipeline/importer interface.
// OzzImporter implements a command line tool to convert any source data format
// to vox skeletons and animations. The tool exposes a set of global options
// through the command line, and a json configuration file to tune import
// settings. Reference json configuration is generated at
// src\animation\offline\importer\reference.json.
// To import a new source data format, one will implement the pure virtual
// functions of this interface. All the conversions end error processing are
// done by the tool.
class VOX_ANIMTOOLS_DLL OzzImporter {
public:
    virtual ~OzzImporter() {}

    // Function operator that must be called with main() arguments to start import
    // process.
    int operator()(int _argc, const char** _argv);

    // Loads source data file.
    // Returning false will report and error.
    virtual bool Load(const char* _filename) = 0;

    // Skeleton management.

    // Defines node types that should be considered as skeleton joints.
    struct NodeType {
        bool skeleton : 1;  // Uses skeleton nodes as skeleton joints.
        bool marker : 1;    // Uses marker nodes as skeleton joints.
        bool camera : 1;    // Uses camera nodes as skeleton joints.
        bool geometry : 1;  // Uses geometry nodes as skeleton joints.
        bool light : 1;     // Uses light nodes as skeleton joints.
        bool null : 1;      // Uses null nodes as skeleton joints.
        bool any : 1;       // Uses any node type as skeleton joints, including those
                            // listed above and any other.
    };

    // Import a skeleton from the source data file.
    // Returning false will report and error.
    virtual bool Import(vox::animation::offline::RawSkeleton* _skeleton, const NodeType& _types) = 0;

    // Animations management.

    // Gets the name of all the animations/clips/takes available from the source
    // data file.
    typedef vox::vector<vox::string> AnimationNames;
    virtual AnimationNames GetAnimationNames() = 0;

    // Import animation "_animation_name" from the source data file.
    // The skeleton is provided such that implementation can look for its joints
    // animations.
    // Returning false will report and error.
    virtual bool Import(const char* _animation_name,
                        const vox::animation::Skeleton& _skeleton,
                        float _sampling_rate,
                        RawAnimation* _animation) = 0;

    // Tracks / properties management.

    // Defines properties, aka user-channel data: animations that aren't only
    // joint transforms.
    struct NodeProperty {
        vox::string name;

        enum Type { kFloat1, kFloat2, kFloat3, kFloat4, kPoint, kVector };
        Type type;
    };

    // Get all properties available for a node.
    typedef vox::vector<NodeProperty> NodeProperties;
    virtual NodeProperties GetNodeProperties(const char* _node_name) = 0;

    // Imports a track of type 1, 2, 3 or 4 floats, for the triplet
    // _animation_name/_node_name/_track_name.
    // Returning false will report and error.
    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _track_type,
                        float _sampling_rate,
                        RawFloatTrack* _track) = 0;
    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _track_type,
                        float _sampling_rate,
                        RawFloat2Track* _track) = 0;
    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _track_type,
                        float _sampling_rate,
                        RawFloat3Track* _track) = 0;
    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _track_type,
                        float _sampling_rate,
                        RawFloat4Track* _track) = 0;

    // Build a filename from a wildcard string.
    vox::string BuildFilename(const char* _filename, const char* _data_name) const;
};
}  // namespace offline
}  // namespace animation
}  // namespace vox