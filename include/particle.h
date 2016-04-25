#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "sprite.h"
#include "entity.h"

/** @file	particle.h
 *	@brief	Particle System, handles randomly loading particles and killing them after cycling through their animation
 */

/**
 * @struct	Particle structure
 * @brief	Defines the particles to be a sprite with a position, frame number, and usage flag. 
 */
typedef struct Particle_s
{
	Uint8 alive;									/**< the usage flag */
	Vect2d position;								/**< the position of the particle */
	Vect2d velocity;								/**< the velocity of the particle (if needed) */
	Sprite *sprite;									/**< the particle's sprite */
	int frameNumber;								/**< frame number of the particle, dies after the frame excceds 30 */
	void (*think) (struct Particle_s *particle);	/**< the think function, if needed of the particle*/
}Particle;

/**
 * @struct Pak of sprites for particles to load from
 * @brief Defines three sprite sheets and a name for the pak to identify each pak
 */
typedef struct Particle_p
{
	char *name;
	Sprite *red;
	Sprite *green;
	Sprite *blue;
}ParticlePak;

/**
 * @brief	Frees the particle from memory and then frees the pointer to that particle.
 * @param [in,out]	double pointer to the particle.
 */
void particle_free(Particle **particle);

/** @brief Closes the particle system and frees all particles from memory */
void particle_close_system();

/**
 * @brief	Initializes the particle system by allocating space for the particleList.
 * @param	maxParticle	the maximum number of particles for the game saved in a static variable for later use.
 */
void particle_initialize_system(int maxParticle);

/**
 * @brief initializes a particle pak from a given config file using cJSON
 *			
 * @param [in]	pak_config_file	the file that the pak will be defined by
 * @param [out] pak				the pak that will be loaded to
 */
void particle_pak_initialize(char *pak_config_file, ParticlePak *pak);

/**
 * @brief finds the first free space in particleList and sets that particle to be alive, with the standard think function
 * @return		a pointer to the the particle in the particleList that has been created
 */
Particle *particle_new();

/**
 * @brief generates a particle in a more precise location and returns it if further use is required, used for trail effects
 * @param [in,out] generator		the particle's, generator
 * @param offsets					the offset that will be used to position the particle slightly more accurately
 * @return null if it fails, else a pointer to a fully loaded particle with a precise location.
 */
Particle *particle_exact_position_load(Entity *generator, Vect2d offsets);

/**
 * @brief generates a particle randomly based on its generators position and returns it if further use is required, used for broad 
 *        particle effects where position is less important
 * @param [in,out] generator		the particle's, generator
 * @return null if it fails, else a pointer to a fully loaded particle.
 */
Particle *particle_assumed_position_load(Entity *generator);

/**
 * @brief loads a particle (using particle_assumed_position_load), this particle will have the basic think function for particles that 
 *        will randomly set a velocity to move in
 * @param [in] generator			the particle's, generator, passed to particle_assumed_position_load
 * @return null if it fails, else a pointer to a fully loaded particle with a think function
 */
Particle *particle_moving_load(Entity *generator);

/**
 * @brief loads a number of randomly created particles using a generator to give it a position and randomly changing it slightly based 
 *        on frameSize of the generator
 * @param [in,out]	generator	the entity that will be the basis for this particle's position generation
 * @param numParticles			the number of particles that will be created
 */
void particle_bundle_load(Entity *generator, int numParticles);

/**
 * @brief Standard think for particles that randomly generates a velocity and adds it to the position
 * @param particle that will be moving
 */
void particle_think(Particle *particle);

/**
 * @brief	checks if a particle has reached it's last frame
 * @param [in,out]	particle	If non-null, the particle to check.
 * @return	true if the particle is at or above its last frame, else false.
 */
int particle_dead(Particle *particle);

/** @brief	checks every particle in particleList with particle_dead. */
void particle_check_all_dead();

/** @brief	draws every particle to the screen using sprite draw, this needs 
 *			to be used because particles aren't entities and camera won't draw them 
 */
void particle_draw_all();

/** @brief checks all particles and makes them think if they have a think function */
void particle_think_all();

#endif
