//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <random>

#include "vox.render/mesh/buffer_mesh.h"
#include "vox.render/particle/particle_material.h"
#include "vox.render/renderer.h"
#include "vox.render/texture/sampled_texture3d.h"

namespace vox {
class ParticleRenderer : public Renderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    // [USER DEFINED]
    static constexpr float kDefaultSimulationVolumeSize = 32.0f;
    static constexpr uint32_t kMaxParticleCount = (1u << 15u);
    static constexpr uint32_t kBatchEmitCount = std::max(256u, (kMaxParticleCount >> 4u));

    enum class EmitterType : uint32_t { POINT, DISK, SPHERE, BALL, kNumEmitterType };

    enum class SimulationVolume : uint32_t { SPHERE, BOX, NONE, kNumSimulationVolume };

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

    ParticleMaterial &material();

    [[nodiscard]] const uint32_t numAliveParticles() const;

public:
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;

    void _updateBounds(BoundingBox3F &worldBounds) override;

    void update(float deltaTime) override;

public:
    [[nodiscard]] float timeStep() const;

    void setTimeStep(float step);

    [[nodiscard]] SimulationVolume boundingVolumeType() const;

    void setBoundingVolumeType(SimulationVolume vol);

    [[nodiscard]] float bboxSize() const;

    void setBBoxSize(float size);

    [[nodiscard]] float scatteringFactor() const;

    void setScatteringFactor(float factor);

    [[nodiscard]] float vectorFieldFactor() const;

    void setVectorFieldFactor(float factor);

    [[nodiscard]] std::shared_ptr<SampledTexture3D> vectorFieldTexture() const;

    void setVectorFieldTexture(const std::shared_ptr<SampledTexture3D> &field);

    [[nodiscard]] float curlNoiseFactor() const;

    void setCurlNoiseFactor(float factor);

    [[nodiscard]] float curlNoiseScale() const;

    void setCurlNoiseScale(float scale);

    [[nodiscard]] float velocityFactor() const;

    void setVelocityFactor(float factor);

public:
    [[nodiscard]] uint32_t emitCount() const;

    void setEmitCount(uint32_t count);

    [[nodiscard]] EmitterType emitterType() const;

    void setEmitterType(EmitterType type);

    [[nodiscard]] Vector3F emitterPosition() const;

    void setEmitterPosition(const Vector3F &position);

    [[nodiscard]] Vector3F emitterDirection() const;

    void setEmitterDirection(const Vector3F &direction);

    [[nodiscard]] float emitterRadius() const;

    void setEmitterRadius(float radius);

    [[nodiscard]] float particleMinAge() const;

    void setParticleMinAge(float age);

    [[nodiscard]] float particleMaxAge() const;

    void setParticleMaxAge(float age);

private:
    void _onEnable() override;

    void _onDisable() override;

    void _allocBuffer();

    void _generateRandomValues();

public:
    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json &data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer &p_root) override;

private:
    uint32_t _numAliveParticles = 0;

    std::shared_ptr<BufferMesh> _mesh{nullptr};
    std::shared_ptr<ParticleMaterial> _material{nullptr};

    std::random_device _randomDevice;
    std::mt19937 _mt;
    float _minValue = 0.0;
    float _maxValue = 1.0;
    std::vector<float> _randomVec{};
    const std::string _randomBufferProp;

    ParticleSimulationData _simulationData{};
    const std::string _simulationDataProp;

    ParticleEmitterData _emitterData;
    const std::string _emitterDataProp;

    SampledTexture3DPtr _vectorFieldTexture{nullptr};
    const std::string _vectorFieldTextureProp;
    const std::string _vectorFieldSamplerProp;

private:
    uint32_t _read = 0;
    uint32_t _write = 1;
    std::unique_ptr<Buffer> _atomicBuffer[2] = {nullptr, nullptr};
    const std::string _readAtomicBufferProp;
    const std::string _writeAtomicBufferProp;

    std::unique_ptr<Buffer> _appendConsumeBuffer[2] = {nullptr, nullptr};
    const std::string _readConsumeBufferProp;
    const std::string _writeConsumeBufferProp;

    std::unique_ptr<Buffer> _dpBuffer{nullptr};
    const std::string _dpBufferProp;
    std::unique_ptr<Buffer> _sortIndicesBuffer{nullptr};
    const std::string _sortIndicesBufferProp;
};
}  // namespace vox