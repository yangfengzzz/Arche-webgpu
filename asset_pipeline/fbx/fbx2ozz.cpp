//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/fbx/fbx2ozz.h"

#include "vox.base/logging.h"

int main(int _argc, const char** _argv) {
    Fbx2OzzImporter converter;
    return converter(_argc, _argv);
}

Fbx2OzzImporter::Fbx2OzzImporter() : settings_(fbx_manager_), scene_loader_(nullptr) {}

Fbx2OzzImporter::~Fbx2OzzImporter() { vox::Delete(scene_loader_); }

bool Fbx2OzzImporter::Load(const char* _filename) {
    vox::Delete(scene_loader_);
    scene_loader_ = vox::New<vox::animation::offline::fbx::FbxSceneLoader>(_filename, "", fbx_manager_, settings_);

    if (!scene_loader_->scene()) {
        LOGE("Failed to import file {}.", _filename)
        vox::Delete(scene_loader_);
        scene_loader_ = nullptr;
        return false;
    }
    return true;
}
