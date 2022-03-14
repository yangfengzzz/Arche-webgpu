//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_hpp
#define particle_hpp

#include "renderer.h"
#include "particle_material.h"
#include "texture/sampled_texture3d.h"
#include "mesh/buffer_mesh.h"
#include <random>

namespace vox {
class ParticleRenderer : public Renderer {
public:
    // [USER DEFINED]
    static constexpr float kDefaultSimulationVolumeSize = 32.0f;
    static constexpr uint32_t kMaxParticleCount = (1u << 10u);
    static constexpr uint32_t kBatchEmitCount = std::max(256u, (kMaxParticleCount >> 4u));
    
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
    
    struct TParticle {
        Vector4<float> position;
        Vector4<float> velocity;
        float start_age;
        float age;
        float padding0;
        uint32_t id;
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
    
    explicit ParticleRenderer(Entity *entity);
    
    ParticleMaterial& material();
    
    const uint32_t numAliveParticles() const;
    
public:
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
    void update(float deltaTime) override;
        
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
        
    void _generateRandomValues();
    
private:
    uint32_t _numAliveParticles = 0;

    std::shared_ptr<BufferMesh> _mesh{nullptr};
    std::shared_ptr<ParticleMaterial> _material{nullptr};
    
    std::random_device _randomDevice;
    std::mt19937 _mt;
    float _minValue = 0.0;
    float _maxValue = 1.0;
    std::vector<float> _randomVec{};
    ShaderProperty _randomBufferProp;
    
    ParticleSimulationData _simulationData;
    ShaderProperty _simulationDataProp;
    
    ParticleEmitterData _emitterData;
    ShaderProperty _emitterDataProp;
    
    SampledTexture3DPtr _vectorFieldTexture{nullptr};
    ShaderProperty _vectorFieldTextureProp;
    ShaderProperty _vectorFieldSamplerProp;
    
private:
    uint32_t _read = 0;
    uint32_t _write = 1;
    std::unique_ptr<Buffer> _atomicBuffer[2] = {nullptr, nullptr};
    ShaderProperty _readAtomicBufferProp;
    ShaderProperty _writeAtomicBufferProp;
    
    std::unique_ptr<Buffer> _appendConsumeBuffer[2] = {nullptr, nullptr};
    ShaderProperty _readConsumeBufferProp;
    ShaderProperty _writeConsumeBufferProp;
    
    std::unique_ptr<Buffer> _dpBuffer{nullptr};
    ShaderProperty _dpBufferProp;
    std::unique_ptr<Buffer> _sortIndicesBuffer{nullptr};
    ShaderProperty _sortIndicesBufferProp;
};
}

#endif /* particle_hpp */
