//
//  ambient_light.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef ambient_light_hpp
#define ambient_light_hpp

#include "scene_forward.h"
#include "spherical_harmonics3.h"
#include "texture/sampled_texture.h"
#include "shader/shader_property.h"
#include "matrix4x4.h"

namespace vox {
/**
 * Diffuse mode.
 */
enum class DiffuseMode {
        /** Solid color mode. */
        SolidColor,
        
        /** Texture mode. */
        Texture,
        
        /**
         * SH mode
         * @remarks
         * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse objects under distant illumination.
         */
        SphericalHarmonics
};

/**
 * Ambient light.
 */
class AmbientLight {
public:
    struct EnvMapLight {
        Vector3F diffuse;
        float _diffuseAlign;
        float diffuseIntensity;
        float specularIntensity;
        int mipMapLevel;
        Matrix4x4F transformMatrix;
    };
    
    AmbientLight(Scene *value);
        
    /**
     * Diffuse mode of ambient light.
     */
    DiffuseMode diffuseMode();
    
    void setDiffuseMode(DiffuseMode value);
    
    /**
     * Diffuse reflection solid color.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SolidColor`.
     */
    Color diffuseSolidColor();
    
    void setDiffuseSolidColor(const Color &value);
    
    /**
     * Diffuse reflection spherical harmonics 3.
     * @remarks Effective when diffuse reflection mode is `DiffuseMode.SphericalHarmonics`.
     */
    const SphericalHarmonics3 &diffuseSphericalHarmonics();
    
    void setDiffuseSphericalHarmonics(const SphericalHarmonics3 &value);
    
    /**
     * Diffuse reflection texture.
     * @remarks This texture must be baked from MetalLoader::createIrradianceTexture
     */
    std::shared_ptr<SampledTexture> diffuseTexture();
    
    void setDiffuseTexture(std::shared_ptr<SampledTexture> value);
    
    /**
     * Diffuse reflection intensity.
     */
    float diffuseIntensity();
    
    void setDiffuseIntensity(float value);
    
public:
    /**
     * Whether to decode from specularTexture with RGBM format.
     */
    bool specularTextureDecodeRGBM();
    
    void setSpecularTextureDecodeRGBM(bool value);
    
    /**
     * Specular reflection texture.
     * @remarks This texture must be baked from MetalLoader::createSpecularTexture
     */
    std::shared_ptr<SampledTexture> specularTexture();
    
    void setSpecularTexture(std::shared_ptr<SampledTexture> value);
    
    /**
     * Specular reflection intensity.
     */
    float specularIntensity();
    
    void setSpecularIntensity(float value);
    
public:
    /**
     * brdf loopup texture.
     * @remarks This texture must be baked from MetalLoader::createBRDFLookupTable
     */
    std::shared_ptr<SampledTexture> brdfTexture();
    
    void setBRDFTexture(std::shared_ptr<SampledTexture> value);
    
private:
    std::array<float, 27> _preComputeSH(const SphericalHarmonics3 &sh);
    
    EnvMapLight _envMapLight;
    ShaderProperty _envMapProperty;
    
    SphericalHarmonics3 _diffuseSphericalHarmonics;
    std::array<float, 27> _shArray{};
    ShaderProperty _diffuseSHProperty;
    
    std::shared_ptr<SampledTexture> _diffuseTexture{nullptr};
    ShaderProperty _diffuseTextureProperty;
    ShaderProperty _diffuseSamplerProperty;
    
    bool _specularTextureDecodeRGBM{false};
    std::shared_ptr<SampledTexture> _specularReflection{nullptr};
    ShaderProperty _specularTextureProperty;
    ShaderProperty _specularSamplerProperty;

    std::shared_ptr<SampledTexture> _brdfLutTexture{nullptr};
    ShaderProperty _brdfTextureProperty;
    ShaderProperty _brdfSamplerProperty;

    Scene *_scene;
    DiffuseMode _diffuseMode = DiffuseMode::SolidColor;
};

}

#endif /* ambient_light_hpp */
