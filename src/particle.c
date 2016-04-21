#include <stdlib.h>

#include "particle.h"
#include "simple_logger.h"
#include "camera.h"

static Particle *particleList = NULL;	/**< the list of active particles */
static int particleMax;					/**< the maximum number of particles allowed for the game */
static int particleNum;					/**< the number of particles currently running */
static ParticlePak *Dust;

void particle_free(Particle **particle)
{
	Particle *self;
	if(!particle)
	{
		slog("particle isn't pointing to anything");
		return;
	}
	else if(!*particle) 
	{
		slog("particle pointer isn't pointing to anything");
		return;
	}
	self = *particle;
	self->alive--;
	particleNum--;
	// do not free sprite because each particle is using the same sprites
	if(self->sprite != NULL) //test
	{
		sprite_free(&self->sprite);
	}
	memset(self, 0, sizeof (Particle));
}

void particle_close_system()
{
	int i;
	Particle *particle;
	if(!particleList)
	{
		slog("particleList not initialized");
		return;
	}
	for(i = 0; i < particleMax; ++i)
	{
		particle = &particleList[i];
		particle_free(&particle);
	}
	particleList = NULL;
}

void particle_initialize_system(int maxParticle)
{
	int i;
	if(maxParticle == 0)
	{
		slog("maxParticle == 0");
		return;
	}
	particleList = (Particle *) malloc (sizeof (Particle) * maxParticle);
	if(!particleList)
	{
		slog("failed to initialize particleList");
		return;
	}
	memset(particleList, 0, sizeof (Particle) * maxParticle);
	for(i = 0; i < maxParticle; ++i)
	{
		particleList[i].sprite = NULL;
	}
	particleMax = maxParticle;

	//initialize the particlePaks that are global here

	atexit(particle_close_system);
}
