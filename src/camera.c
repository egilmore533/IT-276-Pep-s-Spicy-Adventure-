#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"
#include "simple_logger.h"
#include "camera.h"
#include "graphics.h"
#include "files.h"

static Entity *camera = NULL;
static Uint8 moving = 1; /**< flag to tell the camera to start and stop moving */

static Uint32 gametime = 0;

Entity *camera_load(int id)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *camera_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Vect2d pos = vect2d_new(0,0); //camera should always begin at 0,0
	Vect2d vel = pos; //needs to be initialized, doesn't matter for now so re use position
	Uint32 nextThink, thinkRate;
	int health, type;

	camera_config_file = fopen(CAMERA_CONFIG,"r");
	if(!camera_config_file)
	{
		slog("No file found: %s", CAMERA_CONFIG);
		return;
	}
	//get the length of the file
	fseek(camera_config_file, 0, SEEK_END);
	length = ftell(camera_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(camera_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, camera_config_file);
	fclose(camera_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"camera_config");
	if(!root)
	{
		slog("error parseing the file, file not the camera_config");
		return;
	}
	obj = cJSON_GetObjectItem(root, "camera");

	buf = cJSON_GetObjectItem(obj, "info");

	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	nextThink = cJSON_GetObjectItem(buf, "nextThink")->valueint;

	camera = entity_new(thinkRate, health, vel);
	camera->velocity = camera->maxVelocity;
	camera->bounds = rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	camera->update = &camera_update;
	camera->free = &entity_free;
	camera->nextThink = get_time() + camera->thinkRate;
	camera->id = id;
	gametime = SDL_GetTicks();
	camera_move();
	return camera;
}

void camera_update(Entity *self)
{
	if(moving)
	{
		vect2d_add(self->position, self->velocity, self->position);
	}
	gametime = SDL_GetTicks();
}

Entity *camera_get()
{
	if(camera && camera->alive)
	{
		return camera;
	}
	return NULL;
}

void camera_free_entity_outside_bounds(Entity *ent)
{
	if(!entity_intersect(camera, ent))
	{
		ent->free(&ent);
	}
}

void camera_stop()
{
	moving = 0;
}

void camera_move()
{
	moving = 1;
}

void camera_free(Entity *camera)
{
	entity_free(&camera);
	camera = NULL; //just to be sure
}

Uint8 camera_get_move()
{
	return moving;
}

Uint32 get_time()
{
	return gametime;
}

void camera_editor_think(Entity *self)
{
	const Uint8 *keys;

	//movement input 
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_A])
	{
		camera->velocity.x = -camera->maxVelocity.x;
	}
	else if(keys[SDL_SCANCODE_D])
	{
		camera->velocity.x = camera->maxVelocity.x;
	}
	else
	{
		camera->velocity.x = 0;
	}
}
