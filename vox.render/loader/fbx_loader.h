//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef fbx_loader_hpp
#define fbx_loader_hpp

#include "fbx_mesh.h"
#include <ozz/animation/runtime/skeleton.h>

namespace ozz {
namespace loader {
bool loadScene(const char* _filename, const animation::Skeleton& skeleton,
               vector<Mesh>& _meshes);

// Loads a sample::Mesh from an ozz archive file named _filename.
// This function will fail and return false if the file cannot be opened or if
// it is not a valid ozz mesh archive. A valid mesh archive can be
// serialization API.
// _filename and _mesh must be non-nullptr.
bool loadMesh(const char* _filename, Mesh* _mesh);

// Loads n sample::Mesh from an ozz archive file named _filename.
// This function will fail and return false if the file cannot be opened or if
// it is not a valid ozz mesh archive. A valid mesh archive can be
// produced with ozz tools (fbx2skin) or using ozz animation serialization API.
// _filename and _mesh must be non-nullptr.
bool loadMeshes(const char* _filename, vector<Mesh>* _meshes);

} // loader
} // ozz

#endif /* fbx_loader_hpp */
