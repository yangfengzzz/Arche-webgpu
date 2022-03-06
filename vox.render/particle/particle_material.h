//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_material_hpp
#define particle_material_hpp

#include "material/base_material.h"
#include "vector3.h"

namespace vox {
class ParticleMaterial : public BaseMaterial {
public:
    enum class ColorMode : uint32_t {
        DEFAULT,
        GRADIENT,
        kNumColorMode
    };
    
    struct ParticleData {
        Vector3F birthGradient;
        float minParticleSize;
        Vector3F deathGradient;
        float maxParticleSize;
        ColorMode colorMode = ColorMode::DEFAULT;
        float fadeCoefficient;
        bool debugDraw;
    };
    
    float minParticleSize() const;
    
    void setMinParticleSize(float size);
    
    float maxParticleSize() const;
    
    void setMaxParticleSize(float size);
    
    ColorMode colorMode() const;
    
    void setColorMode(ColorMode mode);
    
    Vector3F birthGradient() const;
    
    void setBirthGradient(const Vector3F& gradient);
    
    Vector3F deathGradient() const;
    
    void setDeathGradient(const Vector3F& gradient);
    
    float fadeCoefficient() const;
    
    void setFadeCoefficient(float coeff);
    
    bool debugDraw() const;
    
    void setDebugDraw(bool flag);
    
    ParticleMaterial(wgpu::Device& device);
    
private:
    ParticleData _particleData;
    ShaderProperty _particleDataProp;
};

}
#endif /* particle_material_hpp */
