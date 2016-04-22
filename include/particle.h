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

typedef struct Particle_p
{
	char *name;
	Sprite *red;
	Sprite *green;
	Sprite *blue;
}ParticlePak;

void particle_free(Particle **particle);

void particle_close_system();

void particle_initialize_system(int maxParticle);

void particle_pak_initialize(char *pak_config_file, ParticlePak *pak);

Particle *particle_new();

Particle *particle_exact_position_load(Entity *generator, Vect2d offsets);

Particle *particle_assumed_position_load(Entity *generator);

void particle_bundle_load(Entity *generator, int numParticles);

void particle_think(Particle *particle);

int particle_dead(Particle *particle);

void particle_check_all_dead();

void particle_draw_all();

void particle_think_all();

#endif
