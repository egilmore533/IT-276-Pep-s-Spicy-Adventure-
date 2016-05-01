#include <stdlib.h>
#include <stdio.h>

#include "power_up.h"
#include "player.h"
#include "simple_logger.h"
#include "cJSON.h"
#include "files.h"

Entity *power_up_load(int type, Entity *power_up)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *power_up_config_file;
	long length;
	char *data;
	char *string;

	//sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	power_up_config_file = fopen(POWER_UP_CONFIG,"r");
	if(!power_up_config_file)
	{
		slog("No file found: %s", POWER_UP_CONFIG);
		return;
	}
	//get the length of the file
	fseek(power_up_config_file, 0, SEEK_END);
	length = ftell(power_up_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(power_up_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, power_up_config_file);
	fclose(power_up_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"power_up_config");
	if(!root)
	{
		slog("error parseing the file, file not the power_up_config");
		return;
	}

	switch(type)
	{
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
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;

	power_up = entity_new(0, 0, vect2d_new(0, 0)); //new entity, but we have no real info for power_ups so just give us a shell of a entity
	power_up->sprite = sprite_load(filepath, frameSize, fpl, frames);
	power_up->bounds = rect(0, 0, power_up->sprite->frameSize.x, power_up->sprite->frameSize.y);
	power_up->frameNumber = frame;

	switch(type)
	{
	case POWER_UP_GOO:
		power_up->touch = &power_up_goo_shot_touch;
		break;
	case POWER_UP_SHIELD:
		power_up->touch = &power_up_heat_shield_touch;
		break;
	case POWER_UP_SPREAD:
		power_up->touch = &power_up_spread_shot_touch;
		break;
	case POWER_UP_DOUBLE:
		power_up->touch = &power_up_double_tap_touch;
		break;
	case POWER_UP_LIFE:
		power_up->touch = &power_up_extra_life_touch;
		break;
	case POWER_UP_BOMB:
		power_up->touch = &power_up_bomb_pickup_touch;
		break;
	}

	power_up->draw = &sprite_draw;
	power_up->update = &power_up_update;
	power_up->target = player_get();
	return power_up;
}

void power_up_update(Entity *power_up)
{
	entity_intersect_all(power_up);
}

void power_up_double_tap_touch(Entity *double_tap, Entity *other)
{
	if(!double_tap->target)
	{
		slog("no double_tap target");
		return;
	}
	if(other == double_tap->target)
	{
		other->thinkRate /= 2;
		double_tap->free(&double_tap);
	}
}

void power_up_heat_shield_touch(Entity *heat_shield, Entity *other)
{
	if(!heat_shield->target)
	{
		slog("no heat_shield target");
		return;
	}
	if(other == heat_shield->target)
	{
		other->state = SHIELDED_STATE;
		other->health = 2;
		SDL_SetTextureAlphaMod(other->sprite->image, 255);
		heat_shield->free(&heat_shield);
	}
}

void power_up_bomb_pickup_touch(Entity *bomb, Entity *other)
{
	if(!bomb->target)
	{
		slog("no bomb target");
		return;
	}
	if(other == bomb->target)
	{
		player_add_bomb();
		bomb->free(&bomb);
	}
}

void power_up_spread_shot_touch(Entity *spread, Entity *other)
{
	if(!spread->target)
	{
		slog("no spread target");
		return;
	}
	if(other == spread->target)
	{
		other->inventory[SPREADS]++;
		spread->free(&spread);
	}
}

void power_up_goo_shot_touch(Entity *sticky_shot, Entity *other)
{
	if(!sticky_shot->target)
	{
		slog("no sticky_shot target");
		return;
	}
	if(other == sticky_shot->target)
	{
		other->bulletState = GOO_SHOT_STATE;
		sticky_shot->free(&sticky_shot);
	}
}

void power_up_extra_life_touch(Entity *extra_life, Entity *other)
{
	if(!extra_life->target)
	{
		slog("no extra_life target");
		return;
	}
	if(other == extra_life->target)
	{
		player_add_life();
		extra_life->free(&extra_life);
	}
}
