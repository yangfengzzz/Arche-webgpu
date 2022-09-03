//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_renderer.h"
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

uint32_t const ParticleRenderer::kBatchEmitCount;

std::string ParticleRenderer::name() {
    return "ParticleRenderer";
}

ParticleRenderer::ParticleRenderer(Entity *entity) :
Renderer(entity),
_randomBufferProp(Shader::createProperty("u_randomBuffer", ShaderDataGroup::Renderer)),
_simulationDataProp(Shader::createProperty("u_simulationData", ShaderDataGroup::Renderer)),
_emitterDataProp(Shader::createProperty("u_emitterData", ShaderDataGroup::Renderer)),
_vectorFieldTextureProp(Shader::createProperty("u_vectorFieldTexture", ShaderDataGroup::Renderer)),
_vectorFieldSamplerProp(Shader::createProperty("u_vectorFieldSampler", ShaderDataGroup::Renderer)),

_readAtomicBufferProp(Shader::createProperty("u_readAtomicBuffer", ShaderDataGroup::Renderer)),
_writeAtomicBufferProp(Shader::createProperty("u_writeAtomicBuffer", ShaderDataGroup::Renderer)),
_readConsumeBufferProp(Shader::createProperty("u_readConsumeBuffer", ShaderDataGroup::Renderer)),
_writeConsumeBufferProp(Shader::createProperty("u_writeConsumeBuffer", ShaderDataGroup::Renderer)),
_dpBufferProp(Shader::createProperty("u_dpBuffer", ShaderDataGroup::Renderer)),
_sortIndicesBufferProp(Shader::createProperty("u_sortIndicesBuffer", ShaderDataGroup::Renderer)) {
    _allocBuffer();
    
    _mesh = std::make_shared<BufferMesh>();
    _mesh->addSubMesh(0, 4, wgpu::PrimitiveTopology::TriangleStrip);
    
    std::vector<wgpu::VertexAttribute> vertexAttributes(3);
    vertexAttributes[0].format = wgpu::VertexFormat::Float32x4;
    vertexAttributes[0].offset = 0;
    vertexAttributes[0].shaderLocation = 0;
    vertexAttributes[1].format = wgpu::VertexFormat::Float32x4;
    vertexAttributes[1].offset = sizeof(Vector4F);
    vertexAttributes[1].shaderLocation = 1;
    vertexAttributes[2].format = wgpu::VertexFormat::Float32x4;
    vertexAttributes[2].offset = 2 * sizeof(Vector4F);
    vertexAttributes[2].shaderLocation = 2;
    _mesh->setVertexLayouts(vertexAttributes, sizeof(TParticle), wgpu::VertexStepMode::Instance);
    
    _material = std::make_shared<ParticleMaterial>(entity->scene()->device());
    setMaterial(_material);
}

void ParticleRenderer::_allocBuffer() {
    auto& device = entity()->scene()->device();
    
    /* Assert than the number of particles will be a factor of threadGroupWidth */
    uint32_t numParticles = ParticleManager::floorParticleCount(kMaxParticleCount); //
    shaderData.enableMacro(PARTICLE_COUNT, numParticles);
    fprintf(stderr, "[ %u particles, %u per batch ]\n", numParticles , kBatchEmitCount);
    
    /* Random value buffer */
    _randomVec.resize(4u * 256);
    shaderData.setData(_randomBufferProp, _randomVec);
        
    /* Atomic */
    _atomicBuffer[0] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc);
    _atomicBuffer[1] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc);
    shaderData.setBufferFunctor(_readAtomicBufferProp, [this]()->Buffer {
        return *_atomicBuffer[_read];
    });
    shaderData.setBufferFunctor(_writeAtomicBufferProp, [this]()->Buffer {
        return *_atomicBuffer[_write];
    });
    
    /* Append Consume */
    _appendConsumeBuffer[0] = std::make_unique<Buffer>(device, sizeof(TParticle) * numParticles,
                                                       wgpu::BufferUsage::Storage | wgpu::BufferUsage::Vertex);
    _appendConsumeBuffer[1] = std::make_unique<Buffer>(device, sizeof(TParticle) * numParticles,
                                                       wgpu::BufferUsage::Storage | wgpu::BufferUsage::Vertex);
    shaderData.setBufferFunctor(_readConsumeBufferProp, [this]()->Buffer {
        return *_appendConsumeBuffer[_read];
    });
    shaderData.setBufferFunctor(_writeConsumeBufferProp, [this]()->Buffer {
        return *_appendConsumeBuffer[_write];
    });
    
    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const sort_buffer_max_count = closestPowerOfTwo(kMaxParticleCount); //
    _dpBuffer = std::make_unique<Buffer>(device, sizeof(float) * sort_buffer_max_count,
                                         wgpu::BufferUsage::Storage);
    _sortIndicesBuffer = std::make_unique<Buffer>(device, sizeof(uint32_t) * sort_buffer_max_count * 2,
                                                  wgpu::BufferUsage::Storage);
    shaderData.setBufferFunctor(_dpBufferProp, [this]()->auto {
        return *_dpBuffer;
    });
    shaderData.setBufferFunctor(_sortIndicesBufferProp, [this]()->auto {
        return *_sortIndicesBuffer;
    });
}

void ParticleRenderer::_generateRandomValues() {
    std::uniform_real_distribution<float> distrib(_minValue, _maxValue);
    for (unsigned int i = 0u; i < _randomVec.size(); ++i) {
        _randomVec[i] = distrib(_mt);
    }
    shaderData.setData(_randomBufferProp, _randomVec);
}

void ParticleRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                               std::vector<RenderElement> &alphaTestQueue,
                               std::vector<RenderElement> &transparentQueue) {
    if (_numAliveParticles > 0) {
        transparentQueue.push_back(RenderElement(this, _mesh, _mesh->subMesh(), _material));
    }
}

void ParticleRenderer::_updateBounds(BoundingBox3F &worldBounds) {
    worldBounds.lowerCorner.x = -std::numeric_limits<float>::max();
    worldBounds.lowerCorner.y = -std::numeric_limits<float>::max();
    worldBounds.lowerCorner.z = -std::numeric_limits<float>::max();
    worldBounds.upperCorner.x = std::numeric_limits<float>::max();
    worldBounds.upperCorner.y = std::numeric_limits<float>::max();
    worldBounds.upperCorner.z = std::numeric_limits<float>::max();
}

void ParticleRenderer::update(float deltaTime) {
    setTimeStep(deltaTime * ParticleManager::getSingleton().timeStepFactor());
    _write = 1 - _write;
    _read = 1 - _read;
    
    // todo
    _mesh->setInstanceCount(_numAliveParticles);
    _mesh->setVertexBufferBinding(*_appendConsumeBuffer[_read]);
    _generateRandomValues();
}

const uint32_t ParticleRenderer::numAliveParticles() const {
    return _numAliveParticles;
}

ParticleMaterial& ParticleRenderer::material() {
    return *_material;
}

float ParticleRenderer::timeStep() const {
    return _simulationData.timeStep;
}

void ParticleRenderer::setTimeStep(float step) {
    _simulationData.timeStep = step;
    shaderData.setData(_simulationDataProp, _simulationData);
}

ParticleRenderer::SimulationVolume ParticleRenderer::boundingVolumeType() const {
    return _simulationData.boundingVolumeType;
}

void ParticleRenderer::setBoundingVolumeType(SimulationVolume type) {
    _simulationData.boundingVolumeType = type;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float ParticleRenderer::bboxSize() const {
    return _simulationData.bboxSize;
}

void ParticleRenderer::setBBoxSize(float size) {
    _simulationData.bboxSize = size;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float ParticleRenderer::scatteringFactor() const {
    return _simulationData.scatteringFactor;
}

void ParticleRenderer::setScatteringFactor(float factor) {
    _simulationData.scatteringFactor = factor;
    shaderData.setData(_simulationDataProp, _simulationData);
    shaderData.enableMacro(NEED_PARTICLE_SCATTERING);
}

std::shared_ptr<SampledTexture3D> ParticleRenderer::vectorFieldTexture() const {
    return _vectorFieldTexture;
}

void ParticleRenderer::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    _vectorFieldTexture = field;
    shaderData.enableMacro(NEED_PARTICLE_VECTOR_FIELD);
    shaderData.setSampledTexture(_vectorFieldTextureProp, _vectorFieldSamplerProp, field);
}

float ParticleRenderer::vectorFieldFactor() const {
    return _simulationData.vectorFieldFactor;
}

void ParticleRenderer::setVectorFieldFactor(float factor) {
    _simulationData.vectorFieldFactor = factor;
    shaderData.setData(_simulationDataProp, _simulationData);
}

float ParticleRenderer::curlNoiseFactor() const {
    return _simulationData.curlNoiseFactor;
}

void ParticleRenderer::setCurlNoiseFactor(float factor) {
    _simulationData.curlNoiseFactor = factor;
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_simulationDataProp, _simulationData);
}

float ParticleRenderer::curlNoiseScale() const {
    return _simulationData.curlNoiseScale;
}

void ParticleRenderer::setCurlNoiseScale(float scale) {
    _simulationData.curlNoiseScale = scale;
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_simulationDataProp, _simulationData);
}

float ParticleRenderer::velocityFactor() const {
    return _simulationData.velocityFactor;
}

void ParticleRenderer::setVelocityFactor(float factor) {
    _simulationData.velocityFactor = factor;
    shaderData.enableMacro(NEED_PARTICLE_VELOCITY_CONTROL);
    shaderData.setData(_simulationDataProp, _simulationData);
}

//MARK: - Emitter
uint32_t ParticleRenderer::emitCount() const {
    return _emitterData.emitCount;
}

void ParticleRenderer::setEmitCount(uint32_t count) {
    _numAliveParticles += count;
    _emitterData.emitCount = count;
    shaderData.setData(_emitterDataProp, _emitterData);
}

ParticleRenderer::EmitterType ParticleRenderer::emitterType() const {
    return _emitterData.emitterType;
}

void ParticleRenderer::setEmitterType(EmitterType type) {
    _emitterData.emitterType = type;
    shaderData.setData(_emitterDataProp, _emitterData);
}

Vector3F ParticleRenderer::emitterPosition() const {
    return _emitterData.emitterPosition;
}

void ParticleRenderer::setEmitterPosition(const Vector3F& position) {
    _emitterData.emitterPosition = position;
    shaderData.setData(_emitterDataProp, _emitterData);
}

Vector3F ParticleRenderer::emitterDirection() const {
    return _emitterData.emitterDirection;
}

void ParticleRenderer::setEmitterDirection(const Vector3F& direction) {
    _emitterData.emitterDirection = direction;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float ParticleRenderer::emitterRadius() const {
    return _emitterData.emitterRadius;
}

void ParticleRenderer::setEmitterRadius(float radius) {
    _emitterData.emitterRadius = radius;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float ParticleRenderer::particleMinAge() const {
    return _emitterData.particleMinAge;
}

void ParticleRenderer::setParticleMinAge(float age) {
    _emitterData.particleMinAge = age;
    shaderData.setData(_emitterDataProp, _emitterData);
}

float ParticleRenderer::particleMaxAge() const {
    return _emitterData.particleMaxAge;
}

void ParticleRenderer::setParticleMaxAge(float age) {
    _emitterData.particleMaxAge = age;
    shaderData.setData(_emitterDataProp, _emitterData);
}

void ParticleRenderer::_onEnable() {
    Renderer::_onEnable();
    ParticleManager::getSingleton().addParticle(this);
}

void ParticleRenderer::_onDisable() {
    Renderer::_onDisable();
    ParticleManager::getSingleton().removeParticle(this);
}

//MARK: - Reflection
void ParticleRenderer::onSerialize(nlohmann::json &data) {
    
}

void ParticleRenderer::onDeserialize(nlohmann::json &data) {
    
}

void ParticleRenderer::onInspector(ui::WidgetContainer& p_root) {
    
}

}
