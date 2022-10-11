//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "asset_pipeline/export.h"
#include "asset_pipeline/fbx.h"
#include "asset_pipeline/importer/import2ozz.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/vector.h"

namespace vox::animation {

class Skeleton;

namespace offline {

struct RawAnimation;
struct RawFloatTrack;
struct RawFloat2Track;
struct RawFloat3Track;
struct RawFloat4Track;
struct RawquaternionTrack;

namespace fbx {

VOX_ASSET_DLL OzzImporter::AnimationNames GetAnimationNames(FbxSceneLoader& _scene_loader);

VOX_ASSET_DLL bool ExtractAnimation(const char* _animation_name,
                                    FbxSceneLoader& _scene_loader,
                                    const Skeleton& _skeleton,
                                    float _sampling_rate,
                                    RawAnimation* _animation);

VOX_ASSET_DLL OzzImporter::NodeProperties GetNodeProperties(FbxSceneLoader& _scene_loader, const char* _node_name);

VOX_ASSET_DLL bool ExtractTrack(const char* _animation_name,
                                const char* _node_name,
                                const char* _track_name,
                                OzzImporter::NodeProperty::Type _type,
                                FbxSceneLoader& _scene_loader,
                                float _sampling_rate,
                                RawFloatTrack* _track);

VOX_ASSET_DLL bool ExtractTrack(const char* _animation_name,
                                const char* _node_name,
                                const char* _track_name,
                                OzzImporter::NodeProperty::Type _type,
                                FbxSceneLoader& _scene_loader,
                                float _sampling_rate,
                                RawFloat2Track* _track);

VOX_ASSET_DLL bool ExtractTrack(const char* _animation_name,
                                const char* _node_name,
                                const char* _track_name,
                                OzzImporter::NodeProperty::Type _type,
                                FbxSceneLoader& _scene_loader,
                                float _sampling_rate,
                                RawFloat3Track* _track);

VOX_ASSET_DLL bool ExtractTrack(const char* _animation_name,
                                const char* _node_name,
                                const char* _track_name,
                                OzzImporter::NodeProperty::Type _type,
                                FbxSceneLoader& _scene_loader,
                                float _sampling_rate,
                                RawFloat4Track* _track);
}  // namespace fbx
}  // namespace offline
}  // namespace vox::animation