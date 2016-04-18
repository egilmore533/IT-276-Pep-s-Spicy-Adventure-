#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"
#include "simple_logger.h"
#include "camera.h"
#include "graphics.h"

static Entity *camera = NULL;
static Uint8 moving = 0; /**< flag to tell the camera to start and stop moving */

void camera_initialize(Vect2d position, int id)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *camera_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Vect2d pos = position; //camera should always begin at 0,0
	Vect2d vel = position; //needs to be initialized, doesn't matter for now so re use position
	Uint32 nextThink, thinkRate;
	int health, type;

	camera_config_file = fopen("def/camera_config.txt","r");
	if(!camera_config_file)
	{
		slog("No file found: %s", "def/camera_config.txt");
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
	sscanf(cJSON_GetObjectItem(buf, "vel")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	nextThink = cJSON_GetObjectItem(buf, "nextThink")->valueint;

	camera = entity_new(nextThink, thinkRate, health, pos, vel);
	camera->bounds = rect(camera->position.x, position.y, WINDOW_WIDTH, WINDOW_HEIGHT);
	camera->update = &camera_update;
}

void camera_update(Entity *self)
{
	if(moving)
	{
		vect2d_add(self->position, self->velocity, self->position);
	}
}

Entity *camera_get()
{
	return camera;
}

void camera_free_entity_outside_bounds(Entity *ent)
{
	if(!entity_intersect(camera, ent))
	{
		ent->free(ent);
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
