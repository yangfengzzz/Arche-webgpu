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
    _randomBuffer = std::make_unique<Buffer>(device, sizeof(float) * num_randvalues, wgpu::BufferUsage::Storage);
    shaderData.setBuffer(_randomBufferProp, *_randomBuffer);
    
    /* Atomic */
    uint32_t atomicInit = 0;
    _atomicBuffer[0] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage);
    _atomicBuffer[0]->uploadData(device, &atomicInit, sizeof(uint32_t));
    _atomicBuffer[1] = std::make_unique<Buffer>(device, sizeof(uint32_t), wgpu::BufferUsage::Storage);
    _atomicBuffer[1]->uploadData(device, &atomicInit, sizeof(uint32_t));
    
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

}
