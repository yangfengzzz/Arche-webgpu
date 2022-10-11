//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/fbx/fbx.h"
#include "vox.animation/offline/tools/import2vox.h"

// fbx2vox is a command line tool that converts an animation imported from a
// fbx document to vox runtime format.
//
// fbx2vox extracts animated joints from a fbx document. Only the animated
// joints whose names match those of the vox runtime skeleton given as argument
// are selected. Keyframes are then optimized, based on command line settings,
// and serialized as a runtime animation to an vox binary archive.
//
// Use fbx2vox integrated help command (fbx2vox --help) for more details
// about available arguments.

class Fbx2OzzImporter : public vox::animation::offline::OzzImporter {
public:
    Fbx2OzzImporter();
    ~Fbx2OzzImporter();

private:
    virtual bool Load(const char* _filename);

    // Skeleton management
    virtual bool Import(vox::animation::offline::RawSkeleton* _skeleton, const NodeType& _types);

    // Animation management
    virtual AnimationNames GetAnimationNames();

    virtual bool Import(const char* _animation_name,
                        const vox::animation::Skeleton& _skeleton,
                        float _sampling_rate,
                        vox::animation::offline::RawAnimation* _animation);

    // Track management
    virtual NodeProperties GetNodeProperties(const char* _node_name);

    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _expected_type,
                        float _sampling_rate,
                        vox::animation::offline::RawFloatTrack* _track);

    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _expected_type,
                        float _sampling_rate,
                        vox::animation::offline::RawFloat2Track* _track);

    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _expected_type,
                        float _sampling_rate,
                        vox::animation::offline::RawFloat3Track* _track);

    virtual bool Import(const char* _animation_name,
                        const char* _node_name,
                        const char* _track_name,
                        NodeProperty::Type _expected_type,
                        float _sampling_rate,
                        vox::animation::offline::RawFloat4Track* _track);

    // Fbx internal helpers
    vox::animation::offline::fbx::FbxManagerInstance fbx_manager_;
    vox::animation::offline::fbx::FbxAnimationIOSettings settings_;
    vox::animation::offline::fbx::FbxSceneLoader* scene_loader_;
};