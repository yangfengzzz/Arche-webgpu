//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_hpp
#define particle_hpp

#include "script.h"
#include "particle_material.h"
#include "texture/sampled_texture3d.h"
#include "mesh/buffer_mesh.h"
#include <random>

namespace vox {
class Particle : public Script {
public:
    // [USER DEFINED]
    static float constexpr kDefaultSimulationVolumeSize = 32.0f;
    static uint32_t constexpr kMaxParticleCount = (1u << 15u);
    static uint32_t constexpr kBatchEmitCount = std::max(256u, (kMaxParticleCount >> 4u));
    
    enum class EmitterType : uint32_t {
        POINT,
        DISK,
        SPHERE,
        BALL,
        kNumEmitterType
    };
    
    enum class SimulationVolume : uint32_t {
        SPHERE,
        BOX,
        NONE,
        kNumSimulationVolume
    };
    
    struct ParticleEmitterData {
        Vector3F emitterPosition;
        uint32_t emitCount;
        Vector3F emitterDirection;
        EmitterType emitterType;
        float emitterRadius;
        float particleMinAge;
        float particleMaxAge;
        float _pad;
    };
    
    struct ParticleSimulationData {
        float timeStep;
        SimulationVolume boundingVolumeType;
        float bboxSize;
        float scatteringFactor;
        float vectorFieldFactor;
        float curlNoiseFactor;
        float curlNoiseScale;
        float velocityFactor;
    };
    
    explicit Particle(Entity *entity);
    
    ParticleMaterial& material();
    
    const uint32_t numAliveParticles() const;
    
    ShaderData shaderData;
    
public:
    void onUpdate(float deltaTime) override;
    
    float timeStepFactor() const;

    void setTimeStepFactor(float factor);
        
public:
    float timeStep() const;
    
    void setTimeStep(float step);
    
    SimulationVolume boundingVolumeType() const;
    
    void setBoundingVolumeType(SimulationVolume vol);
    
    float bboxSize() const;
    
    void setBBoxSize(float size);
    
    float scatteringFactor() const;
    
    void setScatteringFactor(float factor);
    
    float vectorFieldFactor() const;
    
    void setVectorFieldFactor(float factor);
    
    std::shared_ptr<SampledTexture3D> vectorFieldTexture() const;
    
    void setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field);
    
    float curlNoiseFactor() const;
    
    void setCurlNoiseFactor(float factor);
    
    float curlNoiseScale() const;
    
    void setCurlNoiseScale(float scale);
    
    float velocityFactor() const;
    
    void setVelocityFactor(float factor);
    
public:
    uint32_t emitCount() const;
    
    void setEmitCount(uint32_t count);
    
    EmitterType emitterType() const;
    
    void setEmitterType(EmitterType type);
    
    Vector3F emitterPosition() const;
    
    void setEmitterPosition(const Vector3F& position);
    
    Vector3F emitterDirection() const;
    
    void setEmitterDirection(const Vector3F& direction);
    
    float emitterRadius() const;
    
    void setEmitterRadius(float radius);
    
    float particleMinAge() const;
    
    void setParticleMinAge(float age);
    
    float particleMaxAge() const;
    
    void setParticleMaxAge(float age);
        
private:
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _allocBuffer();
    
    void _allocMesh();
    
    void _generateRandomValues();
    
private:
    uint32_t _numAliveParticles = 0;
    float _timeStepFactor = 1.0f;

    std::shared_ptr<ParticleMaterial> _material{nullptr};
    MeshRenderer* _renderer{nullptr};
    std::shared_ptr<BufferMesh> _meshes[2];
    
    std::random_device _randomDevice;
    std::mt19937 _mt;
    float _minValue = 0.0;
    float _maxValue = 1.0;
    std::vector<float> _randomVec{};
    Buffer _randomBuffer;
    ShaderProperty _randomBufferProp;
    
    uint32_t _read = 0;
    uint32_t _write = 1;
    Buffer _atomicBuffer[2];
    ShaderProperty _readAtomicBufferProp;
    ShaderProperty _writeAtomicBufferProp;
    
    Buffer _appendConsumeBuffer[2];
    ShaderProperty _readConsumeBufferProp;
    ShaderProperty _writeConsumeBufferProp;

    ParticleSimulationData _simulationData;
    ShaderProperty _simulationDataProp;
    
    ParticleEmitterData _emitterData;
    ShaderProperty _emitterDataProp;
    
    SampledTexture3DPtr _vectorFieldTexture{nullptr};
    ShaderProperty _vectorFieldTextureProp;
    ShaderProperty _vectorFieldSamplerProp;
    
    Buffer _dpBuffer;
    Buffer _sortIndicesBuffer;
};
}

#endif /* particle_hpp */
