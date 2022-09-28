//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <fbxsdk.h>

#include "vox.animation/offline/fbx/export.h"
#include "vox.math/scalable_transform.h"
#include "vox.simd_math/simd_math.h"

namespace vox {
struct ScalableTransform;
namespace animation {
namespace offline {
namespace fbx {

// Manages FbxManager instance.
class VOX_ANIMFBX_DLL FbxManagerInstance {
public:
    // Instantiates FbxManager.
    FbxManagerInstance();

    // Destroys FbxManager.
    ~FbxManagerInstance();

    // Cast operator to get internal FbxManager instance.
    operator FbxManager*() const { return fbx_manager_; }

private:
    FbxManager* fbx_manager_;
};

// Default io settings used to import a scene.
class VOX_ANIMFBX_DLL FbxDefaultIOSettings {
public:
    // Instantiates default settings.
    explicit FbxDefaultIOSettings(const FbxManagerInstance& _manager);

    // De-instantiates default settings.
    virtual ~FbxDefaultIOSettings();

    // Get FbxIOSettings instance.
    FbxIOSettings* settings() const { return io_settings_; }

    // Cast operator to get internal FbxIOSettings instance.
    operator FbxIOSettings*() const { return io_settings_; }

private:
    FbxIOSettings* io_settings_;
};

// Io settings used to import an animation from a scene.
class VOX_ANIMFBX_DLL FbxAnimationIOSettings : public FbxDefaultIOSettings {
public:
    FbxAnimationIOSettings(const FbxManagerInstance& _manager);
};

// Io settings used to import a skeleton from a scene.
class VOX_ANIMFBX_DLL FbxSkeletonIOSettings : public FbxDefaultIOSettings {
public:
    FbxSkeletonIOSettings(const FbxManagerInstance& _manager);
};

// Implements axis system and unit system conversion helper, from any Fbx system
// to vox system (FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd,
// FbxAxisSystem::eRightHanded, meter).
// While Fbx sdk FbxAxisSystem::ConvertScene and FbxSystem::ConvertScene only
// affect scene root, this class functions can be used to bake nodes, vertices,
// animations transformations...
class VOX_ANIMFBX_DLL FbxSystemConverter {
public:
    // Initialize converter with fbx scene systems.
    FbxSystemConverter(const FbxAxisSystem& _from_axis, const FbxSystemUnit& _from_unit);

    // Converts a fbx matrix to an vox Float4x4 matrix, in vox axis and unit
    // systems, using _m' = C * _m * (C-1) operation.
    math::Float4x4 ConvertMatrix(const FbxAMatrix& _m) const;

    // Converts fbx matrix to an vox transform, in vox axis and unit systems,
    // using _m' = C * _m * (C-1) operation.
    // Can return false if matrix isn't affine.
    bool ConvertTransform(const FbxAMatrix& _m, math::Transform* _transform) const;

    // Converts fbx FbxVector4 point to an vox Float3, in vox axis and unit
    // systems, using _p' = C * _p operation.
    Vector3F ConvertPoint(const FbxVector4& _p) const;

    // Converts fbx FbxVector4 vector to an vox Float3, in vox axis and unit
    // systems, using _p' = ((C-1)-T) * _p operation. Normals are converted
    // using the inverse transpose matrix to support non-uniform scale
    // transformations.
    Vector3F ConvertVector(const FbxVector4& _p) const;

private:
    // The matrix used to convert from "from" axis/unit to vox coordinate system
    // base.
    math::Float4x4 convert_;

    // The inverse of convert_ matrix.
    math::Float4x4 inverse_convert_;

    // The transpose inverse of convert_ matrix.
    math::Float4x4 inverse_transpose_convert_;
};

// Loads a scene from a Fbx file.
class VOX_ANIMFBX_DLL FbxSceneLoader {
public:
    // Loads the scene that can then be obtained with scene() function.
    FbxSceneLoader(const char* _filename,
                   const char* _password,
                   const FbxManagerInstance& _manager,
                   const FbxDefaultIOSettings& _io_settings);

    FbxSceneLoader(FbxStream* _stream,
                   const char* _password,
                   const FbxManagerInstance& _manager,
                   const FbxDefaultIOSettings& _io_settings);

    ~FbxSceneLoader();

    // Returns a valid scene if fbx import was successful, nullptr otherwise.
    FbxScene* scene() const { return scene_; }

    // Returns a valid converter if fbx import was successful, nullptr otherwise.
    FbxSystemConverter* converter() { return converter_; }

private:
    void ImportScene(FbxImporter* _importer,
                     const bool _initialized,
                     const char* _password,
                     const FbxManagerInstance& _manager,
                     const FbxDefaultIOSettings& _io_settings);

    // Scene instance that was loaded from the file.
    FbxScene* scene_;

    // Axis and unit conversion helper.
    FbxSystemConverter* converter_;
};
}  // namespace fbx
}  // namespace offline
}  // namespace animation
}  // namespace vox