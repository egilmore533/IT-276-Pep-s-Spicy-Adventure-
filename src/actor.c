#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

#include "files.h"
#include "actor.h"
#include "camera.h"
#include "simple_logger.h"

static Actor *actorList = NULL;
static int actorNum;
static int actorMax = 0;

void actor_empty_list()
{
	int i;
	Actor *actor = NULL;
	if(!actorList)
	{
		slog("actorList not initialized");
		return;
	}
	for(i = 0; i < actorMax; ++i)
	{
		actor = &actorList[i];
		actor_free(&actor);
	}
	memset(actorList, 0, sizeof(Actor) * actorMax);
	for(i = 0; i < actorMax; i++)
	{
		if(actorList[i].sprite != NULL)
		{
			sprite_free(&actorList[i].sprite);
		}
	}
	actorNum = 0;
}

void actor_free(Actor **actor)
{
	Actor *target;
	if(!actor)
	{
		return;
	}
	else if(!*actor)
	{
		return;
	}
	target = *actor;
	target->alive--;

	if(target->sprite != NULL)
	{
		sprite_free(&target->sprite);
	}
	actorNum--;
	*actor = NULL;
}

void actor_close_system()
{
	int i;
	Actor *actor;
	if(!actorList)
	{
		slog("actorList not initialized");
		return;
	}
	for(i = 0; i < actorMax; i++)
	{
		actor = &actorList[i];
		actor_free(&actor);
	}
	actorNum = 0;
	free(actorList);
	actorList = NULL;
}

void actor_initialize_system(int maxActor)
{
	int i;
	if(maxActor == 0)
	{
		slog("max Actor == 0");
		return;
	}
	actorList = (Actor *) malloc (sizeof (Actor) * maxActor);
	if(!actorList)
	{
		slog("failed to initialize actorList");
		return;
	}
	memset(actorList, 0, sizeof (Actor) * maxActor);
	actorNum = 0;
	for(i = 0; i < maxActor; i++)
	{
		actorList[i].sprite = NULL;
	}
	actorMax = maxActor;
	atexit(actor_close_system);
}

Actor *actor_new(Vect2d position, Uint8 type)
{
	int i;

	if(actorNum + 1 > actorMax)
	{
		slog("Maximum actors reached");
		exit(1);
	}
	for(i = 0; i < actorMax; i++)
	{
		if(actorList[i].alive)
		{
			continue;
		}
		memset(&actorList[i],0,sizeof(Actor));
		actorList[i].alive = 1;
		actorList[i].type = type;
		actorList[i].position = position;

		actorList[i].sprite = actor_load_sprite(type);

		actorList[i].draw = &sprite_draw;
		//actorList[i].update = &sprite_draw;
		actorList[i].free = &actor_free;
		return &actorList[i];
	}
	return NULL;
}

void actor_draw_all()
{
	int i;
	if(!actorList)
	{
		return;
	}
	for(i = 0; i < actorMax; i++)
	{
		if(!actorList[i].alive)
		{
			continue;
		}
		if(!actorList[i].draw)
		{
			continue;
		}
		actorList[i].draw(actorList[i].sprite, 0, actorList[i].position);
	}
}

void actor_update_all()
{
	int i;
	for(i = 0; i < actorMax; i++)
	{
		if(!actorList[i].alive)
		{
			continue;
		}
		if(!actorList[i].update)
		{
			continue;
		}
		actorList[i].update(&actorList[i]);
	}
}

Sprite *actor_load_sprite(Uint8 type)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *actor_config_file;
	long length;
	char *data;
	char *string;

	//actor sprite info
	Sprite *sprite = NULL;
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	actor_config_file = fopen(ACTOR_CONFIG,"r");

	//get the length of the file
	fseek(actor_config_file, 0, SEEK_END);
	length = ftell(actor_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(actor_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, actor_config_file);
	fclose(actor_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"actor_config");

	if(!root)
	{
		slog("error parseing the file, file not a actor_config");
		return NULL;
	}

	switch(type)
	{
	case ENEMY_CLARENCE:
		obj = cJSON_GetObjectItem(root, "clarence");
		break;
	case ENEMY_MELT:
		obj = cJSON_GetObjectItem(root, "melt");
		break;
	case ENEMY_MILK:
		obj = cJSON_GetObjectItem(root, "milk_tank");
		break;
	case ENEMY_CELERY:
		obj = cJSON_GetObjectItem(root, "celery_stalker");
		break;
	case ENEMY_PROFESSOR:
		obj = cJSON_GetObjectItem(root, "professor_slice");
		break;
	case POWER_UP_GOO:
		obj = cJSON_GetObjectItem(root, "power_up_goo_shot");
		break;
	case POWER_UP_SHIELD:
		obj = cJSON_GetObjectItem(root, "power_up_shield");
		break;
	case POWER_UP_SPREAD:
		obj = cJSON_GetObjectItem(root, "power_up_spread_shot");
		break;
	case POWER_UP_DOUBLE:
		obj = cJSON_GetObjectItem(root, "power_up_double_tap");
		break;
	case POWER_UP_LIFE:
		obj = cJSON_GetObjectItem(root, "power_up_extra_life");
		break;
	case POWER_UP_BOMB:
		obj = cJSON_GetObjectItem(root, "power_up_extra_bomb");
		break;
	}

	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;

	sprite = sprite_load(filepath, frameSize, fpl, frames);
	return sprite;
}

Uint8 actor_get_number_of_type(Uint8 type)
{
	int i;
	Uint8 totalOfType = 0;
	if(!actorList)
	{
		return;
	}
	for(i = 0; i < actorMax; i++)
	{
		if(!actorList[i].alive)
		{
			continue;
		}
		if(actorList[i].type != type)
		{
			continue;
		}
		totalOfType++;
	}
	return totalOfType;
}

Vect2d actor_get_all_positions_of_type(Uint8 type)
{
	static int i = 0;
	static Uint8 currentType = 0;
	Vect2d pos = vect2d_new(0, 0);

	if(type != currentType)
	{
		i = 0;
		currentType = type;
	}
	while(i < actorMax)
	{
		if(!actorList[i].alive)
		{
			i++;
			continue;
		}
		if(actorList[i].type != currentType)
		{
			i++;
			continue;
		}
		pos = actorList[i].position;
		i++;
		break;
	}
	return pos;
}
