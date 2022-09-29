//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/skin.h"
#include "vox.render/mesh/mesh_renderer.h"

namespace vox {
class SkinMeshRenderer : public MeshRenderer {
public:
    // Loads n Skins from an ozz archive file named _filename.
    // This function will fail and return false if the file cannot be opened or if
    // it is not a valid ozz mesh archive. A valid mesh archive can be
    // produced with ozz tools (fbx2skin) or using ozz animation serialization API.
    // _filename and _mesh must be non-nullptr.
    bool loadSkins(const char* _filename);

private:
    vox::vector<Skin> _skins;
};
}  // namespace vox