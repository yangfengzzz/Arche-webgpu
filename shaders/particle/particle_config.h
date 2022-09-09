#ifndef SHADERS_SPARKLE_INTEROP_H_
#define SHADERS_SPARKLE_INTEROP_H_
// ----------------------------------------------------------------------------

// Kernel group width used across the particles pipeline.
#define PARTICLES_KERNEL_GROUP_WIDTH        256u

// ----------------------------------------------------------------------------

// Decide which structure layout to use.
#ifndef SPARKLE_USE_SOA_LAYOUT
#define SPARKLE_USE_SOA_LAYOUT              FALSE
#endif

/// @note
/// GPUParticle should hold all the main buffer objects it uses and give
/// references to subclasses.
#if SPARKLE_USE_SOA_LAYOUT
	#define STORAGE_BINDING_PARTICLE_POSITIONS_A             0
	#define STORAGE_BINDING_PARTICLE_VELOCITIES_A            1
	#define STORAGE_BINDING_PARTICLE_ATTRIBUTES_A            2

	#define STORAGE_BINDING_PARTICLE_POSITIONS_B             3
	#define STORAGE_BINDING_PARTICLE_VELOCITIES_B            4
	#define STORAGE_BINDING_PARTICLE_ATTRIBUTES_B            5

	#define STORAGE_BINDING_RANDOM_VALUES                    6
	#define STORAGE_BINDING_INDIRECT_ARGS                    7
	#define STORAGE_BINDING_DOT_PRODUCTS                     8
	#define STORAGE_BINDING_INDICES_FIRST                    9
	#define STORAGE_BINDING_INDICES_SECOND                  10

	#define COUNT_STORAGE_BINDING                           11
#else
	#define STORAGE_BINDING_PARTICLES_FIRST                  0
	#define STORAGE_BINDING_PARTICLES_SECOND                 1

	#define STORAGE_BINDING_RANDOM_VALUES                    2
	#define STORAGE_BINDING_INDIRECT_ARGS                    3
	#define STORAGE_BINDING_DOT_PRODUCTS                     4
	#define STORAGE_BINDING_INDICES_FIRST                    5
	#define STORAGE_BINDING_INDICES_SECOND                   6

	#define COUNT_STORAGE_BINDING                            7
#endif

// ----------------------------------------------------------------------------

#define ATOMIC_COUNTER_BINDING_FIRST                     12
#define ATOMIC_COUNTER_BINDING_SECOND                    13

// ----------------------------------------------------------------------------

struct TParticle {
  vec4 position;
  vec4 velocity;
  float start_age;
  float age;
  float _padding0;
  uint id;
};

#endif  // SHADERS_SPARKLE_INTEROP_H_
