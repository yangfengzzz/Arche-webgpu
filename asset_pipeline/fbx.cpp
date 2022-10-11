//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/fbx.h"

#include "vox.base/logging.h"
#include "vox.base/memory/allocator.h"

namespace vox::animation::offline::fbx {

FbxManagerInstance::FbxManagerInstance() : fbx_manager_(nullptr) {
    // Instantiate Fbx manager, mostly a memory manager.
    fbx_manager_ = FbxManager::Create();

    // Logs SDK version.
    LOGI("FBX importer version {}.", fbx_manager_->GetVersion())
}

FbxManagerInstance::~FbxManagerInstance() {
    // Destroy the manager and all associated objects.
    fbx_manager_->Destroy();
    fbx_manager_ = nullptr;
}

FbxDefaultIOSettings::FbxDefaultIOSettings(const FbxManagerInstance& _manager) : io_settings_(nullptr) {
    io_settings_ = FbxIOSettings::Create(_manager, IOSROOT);
    io_settings_->SetBoolProp(IMP_FBX_MATERIAL, false);
    io_settings_->SetBoolProp(IMP_FBX_TEXTURE, false);
    io_settings_->SetBoolProp(IMP_FBX_MODEL, false);
    io_settings_->SetBoolProp(IMP_FBX_SHAPE, false);
    io_settings_->SetBoolProp(IMP_FBX_LINK, false);
    io_settings_->SetBoolProp(IMP_FBX_GOBO, false);
}

FbxDefaultIOSettings::~FbxDefaultIOSettings() {
    io_settings_->Destroy();
    io_settings_ = nullptr;
}

FbxAnimationIOSettings::FbxAnimationIOSettings(const FbxManagerInstance& _manager) : FbxDefaultIOSettings(_manager) {}

FbxSkeletonIOSettings::FbxSkeletonIOSettings(const FbxManagerInstance& _manager) : FbxDefaultIOSettings(_manager) {
    settings()->SetBoolProp(IMP_FBX_ANIMATION, false);
}

FbxSceneLoader::FbxSceneLoader(const char* _filename,
                               const char* _password,
                               const FbxManagerInstance& _manager,
                               const FbxDefaultIOSettings& _io_settings)
    : scene_(nullptr), converter_(nullptr) {
    // Create an importer.
    FbxImporter* importer = FbxImporter::Create(_manager, "vox file importer");
    const bool initialized = importer->Initialize(_filename, -1, _io_settings);

    ImportScene(importer, initialized, _password, _manager, _io_settings);

    // Destroy the importer
    importer->Destroy();
}

FbxSceneLoader::FbxSceneLoader(FbxStream* _stream,
                               const char* _password,
                               const FbxManagerInstance& _manager,
                               const FbxDefaultIOSettings& _io_settings)
    : scene_(nullptr), converter_(nullptr) {
    // Create an importer.
    FbxImporter* importer = FbxImporter::Create(_manager, "vox stream importer");
    const bool initialized = importer->Initialize(_stream, nullptr, -1, _io_settings);

    ImportScene(importer, initialized, _password, _manager, _io_settings);

    // Destroy the importer
    importer->Destroy();
}

void FbxSceneLoader::ImportScene(FbxImporter* _importer,
                                 const bool _initialized,
                                 const char* _password,
                                 const FbxManagerInstance& _manager,
                                 const FbxDefaultIOSettings& _io_settings) {
    // Get the version of the FBX file format.
    int major, minor, revision;
    _importer->GetFileVersion(major, minor, revision);

    if (!_initialized)  // Problem with the file to be imported.
    {
        const FbxString error = _importer->GetStatus().GetErrorString();
        LOGE("FbxImporter initialization failed with error: {}", error.Buffer())

        if (_importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion) {
            LOGE("FBX file version is {}.{}.{}", major, minor, revision)
        }
    } else {
        if (_importer->IsFBX()) {
            LOGE("FBX file version is {}.{}.{}", major, minor, revision)
        }

        // Load the scene.
        scene_ = FbxScene::Create(_manager, "vox scene");
        bool imported = _importer->Import(scene_);

        if (!imported &&  // The import file may have a password
            _importer->GetStatus().GetCode() == FbxStatus::ePasswordError) {
            _io_settings.settings()->SetStringProp(IMP_FBX_PASSWORD, _password);
            _io_settings.settings()->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

            // Retries to import the scene.
            imported = _importer->Import(scene_);

            if (!imported && _importer->GetStatus().GetCode() == FbxStatus::ePasswordError) {
                LOGE("Incorrect password.")

                // scene_ will be destroyed because imported is false.
            }
        }

        // Setup axis and system asset_pipeline.
        if (imported) {
            FbxGlobalSettings& settings = scene_->GetGlobalSettings();
            converter_ = New<FbxSystemConverter>(settings.GetAxisSystem(), settings.GetSystemUnit());
        }

        // Clear the scene if import failed.
        if (!imported) {
            scene_->Destroy();
            scene_ = nullptr;
        }
    }
}

FbxSceneLoader::~FbxSceneLoader() {
    if (scene_) {
        scene_->Destroy();
        scene_ = nullptr;
    }

    if (converter_) {
        vox::Delete(converter_);
        converter_ = nullptr;
    }
}

namespace {
vox::simd_math::Float4x4 BuildAxisSystemMatrix(const FbxAxisSystem& _system) {
    int sign;
    vox::simd_math::SimdFloat4 up = vox::simd_math::simd_float4::y_axis();
    vox::simd_math::SimdFloat4 at = vox::simd_math::simd_float4::z_axis();

    // The EUpVector specifies which axis has the up and down direction in the
    // system (typically this is the Y or Z axis). The sign of the EUpVector is
    // applied to represent the direction (1 is up and -1 is down relative to the
    // observer).
    const FbxAxisSystem::EUpVector eup = _system.GetUpVector(sign);
    switch (eup) {
        case FbxAxisSystem::eXAxis: {
            up = simd_math::simd_float4::Load(1.f * sign, 0.f, 0.f, 0.f);
            // If the up axis is X, the remain two axes will be Y And Z, so the
            // ParityEven is Y, and the ParityOdd is Z
            if (_system.GetFrontVector(sign) == FbxAxisSystem::eParityEven) {
                at = simd_math::simd_float4::Load(0.f, 1.f * sign, 0.f, 0.f);
            } else {
                at = simd_math::simd_float4::Load(0.f, 0.f, 1.f * sign, 0.f);
            }
            break;
        }
        case FbxAxisSystem::eYAxis: {
            up = simd_math::simd_float4::Load(0.f, 1.f * sign, 0.f, 0.f);
            // If the up axis is Y, the remain two axes will X And Z, so the
            // ParityEven is X, and the ParityOdd is Z
            if (_system.GetFrontVector(sign) == FbxAxisSystem::eParityEven) {
                at = simd_math::simd_float4::Load(1.f * sign, 0.f, 0.f, 0.f);
            } else {
                at = simd_math::simd_float4::Load(0.f, 0.f, 1.f * sign, 0.f);
            }
            break;
        }
        case FbxAxisSystem::eZAxis: {
            up = simd_math::simd_float4::Load(0.f, 0.f, 1.f * sign, 0.f);
            // If the up axis is Z, the remain two axes will X And Y, so the
            // ParityEven is X, and the ParityOdd is Y
            if (_system.GetFrontVector(sign) == FbxAxisSystem::eParityEven) {
                at = simd_math::simd_float4::Load(1.f * sign, 0.f, 0.f, 0.f);
            } else {
                at = simd_math::simd_float4::Load(0.f, 1.f * sign, 0.f, 0.f);
            }
            break;
        }
        default: {
            assert(false && "Invalid FbxAxisSystem");
            break;
        }
    }

    // If the front axis and the up axis are determined, the third axis will be
    // automatically determined as the left one. The ECoordSystem enum is a
    // parameter to determine the direction of the third axis just as the
    // EUpVector sign. It determines if the axis system is right-handed or
    // left-handed just as the enum values.
    vox::simd_math::SimdFloat4 right;
    if (_system.GetCoorSystem() == FbxAxisSystem::eRightHanded) {
        right = simd_math::Cross3(up, at);
    } else {
        right = simd_math::Cross3(at, up);
    }

    const vox::simd_math::Float4x4 matrix = {{right, up, at, vox::simd_math::simd_float4::w_axis()}};

    return matrix;
}
}  // namespace

FbxSystemConverter::FbxSystemConverter(const FbxAxisSystem& _from_axis, const FbxSystemUnit& _from_unit) {
    // Build axis system conversion matrix.
    const simd_math::Float4x4 from_matrix = BuildAxisSystemMatrix(_from_axis);

    // Finds unit conversion ratio to meters, where GetScaleFactor() is in cm.
    const float to_meters = static_cast<float>(_from_unit.GetScaleFactor()) * .01f;

    // Builds conversion matrices.
    convert_ = Invert(from_matrix) * simd_math::Float4x4::Scaling(simd_math::simd_float4::Load1(to_meters));
    inverse_convert_ = Invert(convert_);
    inverse_transpose_convert_ = Transpose(inverse_convert_);
}

simd_math::Float4x4 FbxSystemConverter::ConvertMatrix(const FbxAMatrix& _m) const {
    const vox::simd_math::Float4x4 m = {{
            vox::simd_math::simd_float4::Load(static_cast<float>(_m[0][0]), static_cast<float>(_m[0][1]),
                                              static_cast<float>(_m[0][2]), static_cast<float>(_m[0][3])),
            vox::simd_math::simd_float4::Load(static_cast<float>(_m[1][0]), static_cast<float>(_m[1][1]),
                                              static_cast<float>(_m[1][2]), static_cast<float>(_m[1][3])),
            vox::simd_math::simd_float4::Load(static_cast<float>(_m[2][0]), static_cast<float>(_m[2][1]),
                                              static_cast<float>(_m[2][2]), static_cast<float>(_m[2][3])),
            vox::simd_math::simd_float4::Load(static_cast<float>(_m[3][0]), static_cast<float>(_m[3][1]),
                                              static_cast<float>(_m[3][2]), static_cast<float>(_m[3][3])),
    }};
    return convert_ * m * inverse_convert_;
}

Vector3F FbxSystemConverter::ConvertPoint(const FbxVector4& _p) const {
    const simd_math::SimdFloat4 p_in = simd_math::simd_float4::Load(
            static_cast<float>(_p[0]), static_cast<float>(_p[1]), static_cast<float>(_p[2]), 1.f);
    const simd_math::SimdFloat4 p_out = convert_ * p_in;
    Vector3F ret;
    simd_math::Store3PtrU(p_out, &ret.x);
    return ret;
}

Vector3F FbxSystemConverter::ConvertVector(const FbxVector4& _p) const {
    const simd_math::SimdFloat4 p_in = simd_math::simd_float4::Load(
            static_cast<float>(_p[0]), static_cast<float>(_p[1]), static_cast<float>(_p[2]), 0.f);
    const simd_math::SimdFloat4 p_out = inverse_transpose_convert_ * p_in;
    Vector3F ret;
    simd_math::Store3PtrU(p_out, &ret.x);
    return ret;
}

bool FbxSystemConverter::ConvertTransform(const FbxAMatrix& _m, ScalableTransform* _transform) const {
    assert(_transform);

    const simd_math::Float4x4 matrix = ConvertMatrix(_m);

    simd_math::SimdFloat4 translation, rotation, scale;
    if (ToAffine(matrix, &translation, &rotation, &scale)) {
        vox::ScalableTransform transform;
        simd_math::Store3PtrU(translation, &_transform->translation.x);
        simd_math::StorePtrU(simd_math::Normalize4(rotation), &_transform->rotation.x);
        simd_math::Store3PtrU(scale, &_transform->scale.x);
        return true;
    }

    // Failed to decompose matrix, reset transform to identity.
    *_transform = vox::ScalableTransform::identity();
    return false;
}
}  // namespace vox::animation::offline::fbx
