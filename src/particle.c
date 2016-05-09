#include <stdlib.h>
#include <stdio.h>

#include "cJSON.h"
#include "simple_logger.h"

#include "particle.h"
#include "camera.h"
#include "files.h"


/* resource management */
static Particle		*particleList = NULL;	/**< the list of active particles */
static int			particleMax;					/**< the maximum number of particles allowed for the game */
static int			particleNum;					/**< the number of particles currently running */

/* particle paks */
static ParticlePak *Dust= NULL;			/**< the particles that the game will be using exclusively */

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
	Dust = (ParticlePak *) malloc (sizeof (ParticlePak));
	particle_pak_initialize(DUST_PAK_CONFIG, Dust);


	atexit(particle_close_system);
}

void particle_pak_initialize(char *pak_config_file, ParticlePak *pak)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *particle_config_file;
	long length;
	char *data;
	char *string;

	//particlePak sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	particle_config_file = fopen(pak_config_file,"r");
	if(!particle_config_file)
	{
		slog("No file found: %s", pak_config_file);
		return;
	}
	//get the length of the file
	fseek(particle_config_file, 0, SEEK_END);
	length = ftell(particle_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(particle_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, particle_config_file);
	fclose(particle_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"particle_pak_config");
	if(!root)
	{
		slog("error parseing the file, file not a particle_config");
		return;
	}

	obj = cJSON_GetObjectItem(root, "red");
	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;
	pak->red = sprite_load(filepath, frameSize, fpl, frames);

	obj = cJSON_GetObjectItem(root, "green");
	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;
	pak->green = sprite_load(filepath, frameSize, fpl, frames);

	obj = cJSON_GetObjectItem(root, "blue");
	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;
	pak->blue = sprite_load(filepath, frameSize, fpl, frames);
}

Particle *particle_new()
{
	int i;
	if(!particleList)
	{
		slog("particleList not yet initialized");
		return NULL;
	}
	/*makesure we have the room for a new sprite*/
	if(particleNum + 1 > particleMax)
	{
		slog("Maximum Particle's Reached.");
		exit(1);
	}
	for(i = 0; i < particleMax; i++)
	{
		if(particleList[i].alive)
		{
			continue;
		}
		memset(&particleList[i],0,sizeof(Particle));
		particleList[i].alive = 1;
		particleNum++;
		return &particleList[i];
	}
}

Particle *particle_exact_position_load(Entity *generator, Vect2d offsets)
{
	Particle *particle;
	if(!generator)
	{
		slog("generator doesn't point to anything");
		return NULL;
	}
	particle = particle_new();
	particle->position.x = (generator->position.x - 5 + ( rand() % 25)) + offsets.x;
	particle->position.y = (generator->position.y - 5 + ( rand() % 25)) + offsets.y;

	switch( rand() % 3 )
	{
		case 1:
			particle->sprite = Dust->red; 
			break;
		case 2:
			particle->sprite = Dust->green;
			break;
		case 0:
			particle->sprite = Dust->blue;
			break;
	}
	particle->frameNumber = (rand() % particle->sprite->frames - 2); 

	return particle;
}

Particle *particle_assumed_position_load(Entity *generator)
{
	Particle *particle;
	particle = particle_new();
	particle->position.x = (generator->position.x + (rand() % (int) (generator->sprite->frameSize.x)));
	particle->position.y = (generator->position.y + (rand() % (int) (generator->sprite->frameSize.y)));
	particle->frameNumber = (rand() % 30);

	switch( rand() % 3 )
	{
		case 1:
			particle->sprite = Dust->red; // this should be what it always is for pep's spice but why not do something dumb for no reason
			break;
		case 2:
			particle->sprite = Dust->green;
			break;
		case 0:
			particle->sprite = Dust->blue;
			break;
	}
	return particle;
}

Particle *particle_moving_load(Entity *generator)
{
	Particle *particle = particle_assumed_position_load(generator);
	particle->think = &particle_think;
	return particle;
}

void particle_bundle_load(Entity *generator, int numParticles)
{
	Particle *particle;
	int i;
	for(i = 0; i < numParticles; i++)
	{
		particle = particle_assumed_position_load(generator);
	}
	return;
}

void particle_think(Particle *particle)
{
	if(particle->velocity.x == 0 && particle->velocity.y == 0)
	{
		particle->velocity = vect2d_new(rand() % 10, rand() % 10);
		switch(rand() % 3)
		{
			case 0:
				particle->velocity.x = -particle->velocity.x;
				break;
			case 1:
				particle->velocity.y = -particle->velocity.y;
				break;
			case 2:
				vect2d_negate(particle->velocity, particle->velocity);
				break;
		}
	}
	vect2d_add(particle->position, particle->velocity, particle->position);
}

int particle_dead(Particle *particle)
{
	if(!particle)
	{
		slog("particle doesn't point to anything");
		return NULL;
	}
	if(particle->frameNumber >= particle->sprite->frames)
	{
		return 1;
	}
	return 0;
}

void particle_check_all_dead()
{
	int i;
	Particle *particle; // stores address of the entlist address for freeing
	if(!particleList)
	{
		slog("particleList not yet initialized");
		return;
	}
	for(i = 0; i < particleMax; i++)
	{
		if(!particleList[i].alive)
		{
			continue;
		}
		if(particle_dead(&particleList[i]))
		{
			particle = &particleList[i];
			particle_free(&particle);
		}
	}
}

void particle_draw_all()
{
	int i;
	if(!particleList)
	{
		slog("particleList not yet initialized");
		return;
	}
	for(i = 0; i < particleMax; i++)
	{
		if(!particleList[i].alive)
		{
			continue;
		}
		sprite_draw(particleList[i].sprite, particleList[i].frameNumber, particleList[i].position);
		particleList[i].frameNumber++; //no update so handle frame changes here
	}
}

void particle_think_all()
{
	int i;
	if(!particleList)
	{
		slog("particleList not yet initialized");
		return;
	}
	for(i = 0; i < particleMax; i++)
	{
		if(!particleList[i].alive)
		{
			continue;
		}
		if(!particleList[i].think)
		{
			continue;
		}
		particleList[i].think(&particleList[i]);
	}
}