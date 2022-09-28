//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/fbx/fbx.h"
#include "vox.animation/offline/fbx/fbx2vox.h"
#include "vox.animation/offline/fbx/fbx_skeleton.h"
#include "vox.animation/offline/raw_skeleton.h"
#include "vox.base/logging.h"

bool Fbx2OzzImporter::Import(vox::animation::offline::RawSkeleton* _skeleton, const NodeType& _types) {
    if (!_skeleton) {
        return false;
    }

    // Reset skeleton.
    *_skeleton = vox::animation::offline::RawSkeleton();

    if (!scene_loader_) {
        return false;
    }

    if (!vox::animation::offline::fbx::ExtractSkeleton(*scene_loader_, _types, _skeleton)) {
        vox::log::Err() << "Fbx skeleton extraction failed." << std::endl;
        return false;
    }

    return true;
}
