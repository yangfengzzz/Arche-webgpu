//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "asset_pipeline/fbx/fbx.h"
#include "asset_pipeline/importer/import2ozz.h"

// fbx2ozz is a command line tool that converts an animation imported from a
// fbx document to vox runtime format.
//
// fbx2ozz extracts animated joints from a fbx document. Only the animated
// joints whose names match those of the vox runtime skeleton given as argument
// are selected. Keyframes are then optimized, based on command line settings,
// and serialized as a runtime animation to a vox binary archive.
//
// Use fbx2ozz integrated help command (fbx2ozz --help) for more details
// about available arguments.

class Fbx2OzzImporter : public vox::animation::offline::OzzImporter {
public:
    Fbx2OzzImporter();
    ~Fbx2OzzImporter() override;

private:
    bool Load(const char* _filename) override;

    // Skeleton management
    bool Import(vox::animation::offline::RawSkeleton* _skeleton, const NodeType& _types) override;

    // Animation management
    AnimationNames GetAnimationNames() override;

    bool Import(const char* _animation_name,
                const vox::animation::Skeleton& _skeleton,
                float _sampling_rate,
                vox::animation::offline::RawAnimation* _animation) override;

    // Track management
    NodeProperties GetNodeProperties(const char* _node_name) override;

    bool Import(const char* _animation_name,
                const char* _node_name,
                const char* _track_name,
                NodeProperty::Type _expected_type,
                float _sampling_rate,
                vox::animation::offline::RawFloatTrack* _track) override;

    bool Import(const char* _animation_name,
                const char* _node_name,
                const char* _track_name,
                NodeProperty::Type _expected_type,
                float _sampling_rate,
                vox::animation::offline::RawFloat2Track* _track) override;

    bool Import(const char* _animation_name,
                const char* _node_name,
                const char* _track_name,
                NodeProperty::Type _expected_type,
                float _sampling_rate,
                vox::animation::offline::RawFloat3Track* _track) override;

    bool Import(const char* _animation_name,
                const char* _node_name,
                const char* _track_name,
                NodeProperty::Type _expected_type,
                float _sampling_rate,
                vox::animation::offline::RawFloat4Track* _track) override;

    // Fbx internal helpers
    vox::animation::offline::fbx::FbxManagerInstance fbx_manager_;
    vox::animation::offline::fbx::FbxAnimationIOSettings settings_;
    vox::animation::offline::fbx::FbxSceneLoader* scene_loader_;
};