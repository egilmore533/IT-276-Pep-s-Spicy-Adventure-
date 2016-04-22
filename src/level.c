#include <stdio.h>
#include <stdlib.h>

#include "level.h"
#include "audio.h"

#include "camera.h"
#include "player.h"
#include "enemy.h"

#include "cJSON.h"
#include "simple_logger.h"

Entity *level_entity_load(int type, int id)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *entity_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Entity *entity = NULL;
	Uint32 nextThink, thinkRate;
	Vect2d vel;
	int health;

	//sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	//audioPak info
	char *name;
	char *fire1_file;
	char *fire2_file;
	char *death_file;
	char *moving_file;
	int channel;

	entity_config_file = fopen("def/entity_config.txt","r");
	if(!entity_config_file)
	{
		slog("No file found: %s", "def/entity_config.txt");
		return;
	}
	//get the length of the file
	fseek(entity_config_file, 0, SEEK_END);
	length = ftell(entity_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(entity_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, entity_config_file);
	fclose(entity_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"entity_config");
	if(!root)
	{
		slog("error parseing the file, file not the entity_config");
		return;
	}
	switch(type)
	{
		case CAMERA:
			entity = camera_load(id);
			return entity;
		case PLAYER:
			channel = FX_Player;
			obj = cJSON_GetObjectItem(root, "player");
			break;
		case ENEMY_CLARENCE:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "clarence");
			break;
		case ENEMY_MELT:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "melt");
			break;
		case ENEMY_MILK:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "milk_tank");
			break;
		case ENEMY_CELERY:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "celery_stalker");
			break;
		case ENEMY_PROFESSOR:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "professor_slice");
			break;
		/*
		case POWER_UP_GOO:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_goo_shot");
			break;
		case POWER_UP_SHIELD:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_shield");
			break;
		case POWER_UP_SPREAD:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_spread_shot");
			break;
		case POWER_UP_DOUBLE:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_double_tap");
			break;
		case POWER_UP_LIFE:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_extra_life");
			break;
		case POWER_UP_BOMB:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_extra_bomb");
			break;
		*/
		case PROJECTILE_PEP_SPREAD:
			channel = FX_Bullets;
			obj = cJSON_GetObjectItem(root, "pep_spread_shot");
			break;
		case PROJECTILE_PEP_CHARGE:
			channel = FX_Bullets;
			obj = cJSON_GetObjectItem(root, "pep_charge_shot");
			break;
		case PROJECTILE_PEP_BOMB:
			channel = FX_Bullets;
			obj = cJSON_GetObjectItem(root, "pep_bomb");
			break;
		case PROJECTILE_MELT_CREAM:
			channel = FX_Bullets;
			obj = cJSON_GetObjectItem(root, "melt_cream");
			break;
		case PROJECTILE_PROFESSOR_SLICE_BREAD:
			channel = FX_Bullets;
			obj = cJSON_GetObjectItem(root, "professor_slice_bread");
			break;
	}

	buf = cJSON_GetObjectItem(obj, "info");
	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	nextThink = cJSON_GetObjectItem(buf, "nextThink")->valueint;

	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;

	buf = cJSON_GetObjectItem(obj, "audioPak");
	name = cJSON_GetObjectItem(buf, "name")->valuestring;
	fire1_file = cJSON_GetObjectItem(buf, "firing1")->valuestring;
	fire2_file = cJSON_GetObjectItem(buf, "firing2")->valuestring;
	death_file = cJSON_GetObjectItem(buf, "death")->valuestring;
	moving_file = cJSON_GetObjectItem(buf, "moving")->valuestring;

	entity = entity_new(nextThink, thinkRate, health, vel);
	entity->sprite = sprite_load(filepath, frameSize, fpl, frames);
	entity->bounds = rect(0, 0, entity->sprite->frameSize.x, entity->sprite->frameSize.y);
	entity->entitySounds = audio_load_pak(channel, name, fire1_file, fire2_file, death_file, moving_file);
	entity->frameNumber = frame;
	entity->draw = &sprite_draw;
	switch(type)
	{
		case PLAYER:
			entity = player_load(entity);
			buf = cJSON_GetObjectItem(obj, "info");
			entity->inventory[LIVES] = cJSON_GetObjectItem(buf, "lives")->valueint;
			entity->inventory[BOMBS] = cJSON_GetObjectItem(buf, "bombs")->valueint;
			entity->inventory[SPREADS] = cJSON_GetObjectItem(buf, "spreads")->valueint;
			break;
		case ENEMY_CLARENCE:
			entity = clarence_load(entity);
			break;
		case ENEMY_MELT:
			entity = melt_load(entity);
			break;
		case ENEMY_MILK:
			entity = milk_tank_load(entity);
			break;
		case ENEMY_CELERY:
			entity = celery_stalker_load(entity);
			break;
		case ENEMY_PROFESSOR:
			entity = professor_slice_load(entity);
			break;
		/*
		case POWER_UP_GOO:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_goo_shot");
			break;
		case POWER_UP_SHIELD:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_shield");
			break;
		case POWER_UP_SPREAD:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_spread_shot");
			break;
		case POWER_UP_DOUBLE:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_double_tap");
			break;
		case POWER_UP_LIFE:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_extra_life");
			break;
		case POWER_UP_BOMB:
			channel = FX_Enemy;
			obj = cJSON_GetObjectItem(root, "power_up_extra_bomb");
			break;
		*/
	}
	entity->free = &entity_free; // this is the default free function
	return entity;
}