//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/fbx/fbx2vox.h"
#include "vox.animation/offline/fbx/fbx_animation.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.base/logging.h"

Fbx2OzzImporter::AnimationNames Fbx2OzzImporter::GetAnimationNames() {
    if (!scene_loader_) {
        return AnimationNames();
    }
    return vox::animation::offline::fbx::GetAnimationNames(*scene_loader_);
}

bool Fbx2OzzImporter::Import(const char* _animation_name,
                             const vox::animation::Skeleton& _skeleton,
                             float _sampling_rate,
                             vox::animation::offline::RawAnimation* _animation) {
    if (!_animation) {
        return false;
    }

    *_animation = vox::animation::offline::RawAnimation();

    if (!scene_loader_) {
        return false;
    }

    return vox::animation::offline::fbx::ExtractAnimation(_animation_name, *scene_loader_, _skeleton, _sampling_rate,
                                                          _animation);
}

Fbx2OzzImporter::NodeProperties Fbx2OzzImporter::GetNodeProperties(const char* _node_name) {
    if (!scene_loader_) {
        return NodeProperties();
    }

    return vox::animation::offline::fbx::GetNodeProperties(*scene_loader_, _node_name);
}

template <typename _RawTrack>
bool ImportImpl(vox::animation::offline::fbx::FbxSceneLoader* _scene_loader,
                const char* _animation_name,
                const char* _node_name,
                const char* _track_name,
                Fbx2OzzImporter::NodeProperty::Type _type,
                float _sampling_rate,
                _RawTrack* _track) {
    if (!_scene_loader) {
        return false;
    }
    return vox::animation::offline::fbx::ExtractTrack(_animation_name, _node_name, _track_name, _type, *_scene_loader,
                                                      _sampling_rate, _track);
}

bool Fbx2OzzImporter::Import(const char* _animation_name,
                             const char* _node_name,
                             const char* _track_name,
                             Fbx2OzzImporter::NodeProperty::Type _type,
                             float _sampling_rate,
                             vox::animation::offline::RawFloatTrack* _track) {
    return ImportImpl(scene_loader_, _animation_name, _node_name, _track_name, _type, _sampling_rate, _track);
}

bool Fbx2OzzImporter::Import(const char* _animation_name,
                             const char* _node_name,
                             const char* _track_name,
                             Fbx2OzzImporter::NodeProperty::Type _type,
                             float _sampling_rate,
                             vox::animation::offline::RawFloat2Track* _track) {
    return ImportImpl(scene_loader_, _animation_name, _node_name, _track_name, _type, _sampling_rate, _track);
}

bool Fbx2OzzImporter::Import(const char* _animation_name,
                             const char* _node_name,
                             const char* _track_name,
                             Fbx2OzzImporter::NodeProperty::Type _type,
                             float _sampling_rate,
                             vox::animation::offline::RawFloat3Track* _track) {
    return ImportImpl(scene_loader_, _animation_name, _node_name, _track_name, _type, _sampling_rate, _track);
}

bool Fbx2OzzImporter::Import(const char* _animation_name,
                             const char* _node_name,
                             const char* _track_name,
                             Fbx2OzzImporter::NodeProperty::Type _type,
                             float _sampling_rate,
                             vox::animation::offline::RawFloat4Track* _track) {
    return ImportImpl(scene_loader_, _animation_name, _node_name, _track_name, _type, _sampling_rate, _track);
}
