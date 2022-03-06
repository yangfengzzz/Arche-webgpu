//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle.h"
#include "entity.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "particle_manager.h"

namespace vox {
namespace {
uint32_t closestPowerOfTwo(uint32_t const n) {
    uint32_t r = 1u;
    for (uint32_t i = 0u; r < n; r <<= 1u)
        ++i;
    return r;
}

}  // namespace

uint32_t const Particle::kBatchEmitCount;

Particle::Particle(Entity *entity) :
Script(entity),
shaderData(entity->scene()->device()),
_randomBufferProp(Shader::createProperty("u_randomBuffer", ShaderDataGroup::Compute)),
_readAtomicBufferProp(Shader::createProperty("u_readAtomicBuffer", ShaderDataGroup::Compute)),
_writeAtomicBufferProp(Shader::createProperty("u_writeAtomicBuffer", ShaderDataGroup::Compute)),
_readConsumeBufferProp(Shader::createProperty("u_readConsumeBuffer", ShaderDataGroup::Compute)),
_writeConsumeBufferProp(Shader::createProperty("u_writeConsumeBuffer", ShaderDataGroup::Compute)),
_simulationDataProp(Shader::createProperty("u_simulationData", ShaderDataGroup::Compute)),
_emitterDataProp(Shader::createProperty("u_emitterData", ShaderDataGroup::Compute)),
_vectorFieldTextureProp(Shader::createProperty("u_vectorFieldTexture", ShaderDataGroup::Compute)),
_vectorFieldSamplerProp(Shader::createProperty("u_vectorFieldSampler", ShaderDataGroup::Compute)) {
    _allocBuffer();
    _allocMesh();
    
    _renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>(entity->scene()->device());
    _renderer->setMaterial(_material);
}

void Particle::_allocBuffer() {
    auto& device = entity()->scene()->device();
    
    /* Assert than the number of particles will be a factor of threadGroupWidth */
    fprintf(stderr, "[ %u particles, %u per batch ]\n", kMaxParticleCount , kBatchEmitCount);
    
    /* Random value buffer */
    uint32_t const num_randvalues = 3u * kMaxParticleCount;
    _randomVec.resize(num_randvalues);
    shaderData.setData(_randomBufferProp, _randomVec);
        
    /* Atomic */
    _atomicBuffer[0] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage | wgpu::BufferUsage::MapRead);
    _atomicBuffer[1] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage | wgpu::BufferUsage::MapRead);
    
    /* Append Consume */
    _appendConsumeBuffer[0] = std::make_unique<Buffer>(device, sizeof(TParticle) * kMaxParticleCount,
                                                       wgpu::BufferUsage::Storage);
    _appendConsumeBuffer[1] = std::make_unique<Buffer>(device, sizeof(TParticle) * kMaxParticleCount,
                                                       wgpu::BufferUsage::Storage);
    
    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const sort_buffer_max_count = closestPowerOfTwo(kMaxParticleCount); //
    _dpBuffer = std::make_unique<Buffer>(device, sizeof(float) * sort_buffer_max_count,
                                         wgpu::BufferUsage::Storage);
    _sortIndicesBuffer = std::make_unique<Buffer>(device, sizeof(uint32_t) * sort_buffer_max_count * 2,
                                                  wgpu::BufferUsage::Storage);
}

void Particle::_allocMesh() {
    for (int i = 0; i < 2; i++) {
        std::vector<wgpu::VertexAttribute> attributes{3};
        attributes[0].offset = 0;
        attributes[0].format = wgpu::VertexFormat::Float32x3;
        attributes[0].shaderLocation = 0;
        attributes[1].offset = sizeof(Vector4F);
        attributes[1].format = wgpu::VertexFormat::Float32x3;
        attributes[1].shaderLocation = 1;
        attributes[2].offset = 2 * sizeof(Vector4F);
        attributes[2].format = wgpu::VertexFormat::Float32x2;
        attributes[2].shaderLocation = 2;
        
        auto bufferMesh = std::make_shared<BufferMesh>();
        bufferMesh->setVertexLayouts(attributes, sizeof(TParticle));
        bufferMesh->setVertexBufferBinding(*_appendConsumeBuffer[i], 0);
        bufferMesh->addSubMesh(SubMesh());
        _meshes[i] = bufferMesh;
    }
}

void Particle::_generateRandomValues() {
    std::uniform_real_distribution<float> distrib(_minValue, _maxValue);
    for (unsigned int i = 0u; i < _randomVec.size(); ++i) {
        _randomVec[i] = distrib(_mt);
    }
    shaderData.setData(_randomBufferProp, _randomVec);
}

void Particle::onUpdate(float deltaTime) {
    _write = 1 - _write;
    _read = 1 - _read;
    
    _atomicBuffer[_read]->handle().MapAsync(wgpu::MapMode::Read, 0, sizeof(uint32_t), [](WGPUBufferMapAsyncStatus status, void * userdata) {
        if (status == WGPUBufferMapAsyncStatus_Success) {
            Particle* app = static_cast<Particle*>(userdata);
            memcpy(&app->_numAliveParticles, app->_atomicBuffer[app->_read]->handle().GetConstMappedRange(0, 4), 4);
            app->_atomicBuffer[app->_read]->handle().Unmap();
        }
    }, this);
    _meshes[_read]->subMesh()->setCount(_numAliveParticles);
    _renderer->setMesh(_meshes[_read]);
    _generateRandomValues();
}

const uint32_t Particle::numAliveParticles() const {
    return _numAliveParticles;
}

ParticleMaterial& Particle::material() {
    return *_material;
}

float Particle::timeStep() const {
    return _simulationData.timeStep;
}

void Particle::setTimeStep(float step) {
    _simulationData.timeStep = step;
    shaderData.setData(_simulationDataProp, _simulationData);
}

Particle::SimulationVolume Particle::boundingVolumeType() const {
    return _simulationData.boundingVolumeType;
}

void Particle::setBoundingVolumeType(SimulationVolume type) {
    _simulationData.boundingVolumeType = type;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float Particle::bboxSize() const {
    return _simulationData.bboxSize;
}

void Particle::setBBoxSize(float size) {
    _simulationData.bboxSize = size;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float Particle::scatteringFactor() const {
    return _simulationData.scatteringFactor;
}

void Particle::setScatteringFactor(float factor) {
    _simulationData.scatteringFactor = factor;
    shaderData.setData(_simulationDataProp, _simulationData);
    shaderData.enableMacro(NEED_PARTICLE_SCATTERING);
}

std::shared_ptr<SampledTexture3D> Particle::vectorFieldTexture() const {
    return _vectorFieldTexture;
}

void Particle::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    _vectorFieldTexture = field;
    shaderData.enableMacro(NEED_PARTICLE_VECTOR_FIELD);
    shaderData.setSampledTexture(_vectorFieldTextureProp, _vectorFieldSamplerProp, field);
}

float Particle::vectorFieldFactor() const {
    return _simulationData.vectorFieldFactor;
}

void Particle::setVectorFieldFactor(float factor) {
    _simulationData.vectorFieldFactor = factor;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float Particle::curlNoiseFactor() const {
    return _simulationData.curlNoiseFactor;
}

void Particle::setCurlNoiseFactor(float factor) {
    _simulationData.curlNoiseFactor = factor;
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_simulationDataProp, _simulationData);
}

float Particle::curlNoiseScale() const {
    return _simulationData.curlNoiseScale;
}

void Particle::setCurlNoiseScale(float scale) {
    _simulationData.curlNoiseScale = scale;
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_simulationDataProp, _simulationData);
}

float Particle::velocityFactor() const {
    return _simulationData.velocityFactor;
}

void Particle::setVelocityFactor(float factor) {
    _simulationData.velocityFactor = factor;
    shaderData.enableMacro(NEED_PARTICLE_VELOCITY_CONTROL);
    shaderData.setData(_simulationDataProp, _simulationData);
}

//MARK: - Emitter
uint32_t Particle::emitCount() const {
    return _emitterData.emitCount;
}

void Particle::setEmitCount(uint32_t count) {
    _emitterData.emitCount = count;
    shaderData.setData(_emitterDataProp, _emitterData);
}

Particle::EmitterType Particle::emitterType() const {
    return _emitterData.emitterType;
}

void Particle::setEmitterType(EmitterType type) {
    _emitterData.emitterType = type;
    shaderData.setData(_emitterDataProp, _emitterData);
}

Vector3F Particle::emitterPosition() const {
    return _emitterData.emitterPosition;
}

void Particle::setEmitterPosition(const Vector3F& position) {
    _emitterData.emitterPosition = position;
    shaderData.setData(_emitterDataProp, _emitterData);
}

Vector3F Particle::emitterDirection() const {
    return _emitterData.emitterDirection;
}

void Particle::setEmitterDirection(const Vector3F& direction) {
    _emitterData.emitterDirection = direction;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float Particle::emitterRadius() const {
    return _emitterData.emitterRadius;
}

void Particle::setEmitterRadius(float radius) {
    _emitterData.emitterRadius = radius;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float Particle::particleMinAge() const {
    return _emitterData.particleMinAge;
}

void Particle::setParticleMinAge(float age) {
    _emitterData.particleMinAge = age;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float Particle::particleMaxAge() const {
    return _emitterData.particleMaxAge;
}

void Particle::setParticleMaxAge(float age) {
    _emitterData.particleMaxAge = age;
    shaderData.setData(_emitterDataProp, _emitterData);
}

void Particle::_onEnable() {
    Script::_onEnable();
    ParticleManager::getSingleton().addParticle(this);
}

void Particle::_onDisable() {
    Script::_onDisable();
    ParticleManager::getSingleton().removeParticle(this);
}

}
