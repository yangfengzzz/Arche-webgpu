//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/skin_mesh_renderer.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"

namespace vox {
bool SkinMeshRenderer::loadSkins(const char* _filename) {
    assert(_filename);
    LOGI("Loading meshes archive: {}", _filename);
    vox::io::File file(_filename, "rb");
    if (!file.opened()) {
        LOGE("Failed to open mesh file {}.", _filename)
        return false;
    }
    vox::io::IArchive archive(&file);

    while (archive.TestTag<Skin>()) {
        _skins.resize(_skins.size() + 1);
        archive >> _skins.back();
    }

    return true;
}

}  // namespace vox