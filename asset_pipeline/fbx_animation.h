//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/fbx/export.h"
#include "vox.animation/offline/fbx/fbx.h"
#include "vox.animation/offline/tools/import2vox.h"
#include "vox.base/containers/string.h"
#include "vox.base/containers/vector.h"

namespace vox {
namespace animation {

class Skeleton;

namespace offline {

struct RawAnimation;
struct RawFloatTrack;
struct RawFloat2Track;
struct RawFloat3Track;
struct RawFloat4Track;
struct RawquaternionTrack;

namespace fbx {

VOX_ANIMFBX_DLL OzzImporter::AnimationNames GetAnimationNames(FbxSceneLoader& _scene_loader);

VOX_ANIMFBX_DLL bool ExtractAnimation(const char* _animation_name,
                                      FbxSceneLoader& _scene_loader,
                                      const Skeleton& _skeleton,
                                      float _sampling_rate,
                                      RawAnimation* _animation);

VOX_ANIMFBX_DLL OzzImporter::NodeProperties GetNodeProperties(FbxSceneLoader& _scene_loader, const char* _node_name);

VOX_ANIMFBX_DLL bool ExtractTrack(const char* _animation_name,
                                  const char* _node_name,
                                  const char* _track_name,
                                  OzzImporter::NodeProperty::Type _type,
                                  FbxSceneLoader& _scene_loader,
                                  float _sampling_rate,
                                  RawFloatTrack* _track);

VOX_ANIMFBX_DLL bool ExtractTrack(const char* _animation_name,
                                  const char* _node_name,
                                  const char* _track_name,
                                  OzzImporter::NodeProperty::Type _type,
                                  FbxSceneLoader& _scene_loader,
                                  float _sampling_rate,
                                  RawFloat2Track* _track);

VOX_ANIMFBX_DLL bool ExtractTrack(const char* _animation_name,
                                  const char* _node_name,
                                  const char* _track_name,
                                  OzzImporter::NodeProperty::Type _type,
                                  FbxSceneLoader& _scene_loader,
                                  float _sampling_rate,
                                  RawFloat3Track* _track);

VOX_ANIMFBX_DLL bool ExtractTrack(const char* _animation_name,
                                  const char* _node_name,
                                  const char* _track_name,
                                  OzzImporter::NodeProperty::Type _type,
                                  FbxSceneLoader& _scene_loader,
                                  float _sampling_rate,
                                  RawFloat4Track* _track);
}  // namespace fbx
}  // namespace offline
}  // namespace animation
}  // namespace vox