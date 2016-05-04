#include <stdio.h>
#include <stdlib.h>

#include "background.h"
#include "level.h"
#include "audio.h"

#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "power_up.h"

#include <glib.h>
#include "cJSON.h"
#include "simple_logger.h"

Entity *level_entity_load(int type, int id)
{
	Entity *entity = NULL;
	switch(type)
	{
		case CAMERA:
			entity = camera_load(id);
			break;
		case PLAYER:
			entity = player_load();
			break;
		case ENEMY_CLARENCE:
			entity = enemy_load(ENEMY_CLARENCE, entity);
			break;
		case ENEMY_MELT:
			entity = enemy_load(ENEMY_MELT, entity);
			break;
		case ENEMY_MILK:
			entity = enemy_load(ENEMY_MILK, entity);
			break;
		case ENEMY_CELERY:
			entity = enemy_load(ENEMY_CELERY, entity);
			break;
		case ENEMY_PROFESSOR:
			entity = enemy_load(ENEMY_PROFESSOR, entity);
			break;
		case POWER_UP_GOO:
			entity = power_up_load(POWER_UP_GOO, entity);
			break;
		case POWER_UP_SHIELD:
			entity = power_up_load(POWER_UP_SHIELD, entity);
			break;
		case POWER_UP_SPREAD:
			entity = power_up_load(POWER_UP_SPREAD, entity);
			break;
		case POWER_UP_DOUBLE:
			entity = power_up_load(POWER_UP_DOUBLE, entity);
			break;
		case POWER_UP_LIFE:
			entity = power_up_load(POWER_UP_LIFE, entity);
			break;
		case POWER_UP_BOMB:
			entity = power_up_load(POWER_UP_BOMB, entity);
			break;
	}
	entity->id = id;
	return entity;
}

void level_free(Level **level)
{
	Level *self;
	Background *selfBackground;
	if(!level)
	{
		return;
	}
	else if(!*level) 
	{
		return;
	}
	self = *level;

	if(self->background != NULL)
	{
		background_pak_free(self->background);
	}
	if(self->backgroundMusic != NULL)
	{
		audio_music_free(&self->backgroundMusic);
	}
	if(self->player != NULL && self->player->free)
	{
		self->player->free(&self->player);
	}
	if(self->cam != NULL && self->cam->free)
	{
		self->cam->free(&self->cam);
	}
	*level = NULL;
}

Level *level_load(char *level_def_file)
{
	Level *newLevel = NULL;

	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *level_config_file;
	long length;
	char *data;

	//bacground music stuff
	char *music_def_file = NULL;
	int loop;

	//entity loading stuff
	int i;
	int id = 0;
	int entityNum;
	Entity *currentEntity = NULL;
	char positionString[100];
	Vect2d currentPos;

	//background loading stuff
	char *background_def_file = NULL;

	newLevel = (Level *) malloc (sizeof (Level));
	memset(newLevel, 0, sizeof (Level));
	
	level_config_file = fopen(level_def_file,"r");
	if(!level_config_file)
	{
		slog("No file found: %s", level_config_file);
		return NULL;
	}
	//get the length of the file
	fseek(level_config_file, 0, SEEK_END);
	length = ftell(level_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(level_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, level_config_file);
	fclose(level_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"level");

	if(!root)
	{
		slog("error parseing the file, file not a level config");
		return NULL;
	}
	//cam first ALWAYS
	obj = cJSON_GetObjectItem(root, "camera");
	sscanf(cJSON_GetObjectItem(obj, "position")->valuestring, "%f %f", &currentPos.x, &currentPos.y);
	currentEntity = level_entity_load(CAMERA, id++);
	currentEntity->position = currentPos;
	newLevel->cam = currentEntity;

	//music load
	obj = cJSON_GetObjectItem(root, "music");
	music_def_file = cJSON_GetObjectItem(obj, "file")->valuestring;
	loop = cJSON_GetObjectItem(obj, "loop")->valueint;
	newLevel->backgroundMusic = audio_load_music(music_def_file, loop);
	audio_play_music(newLevel->backgroundMusic);

	//background load
	obj = cJSON_GetObjectItem(root, "background");
	background_def_file = cJSON_GetObjectItem(obj, "file")->valuestring;
	newLevel->background = background_pak_new(background_def_file);

	//entity load
	obj = cJSON_GetObjectItem(root, "entities");

	//player
	obj = cJSON_GetObjectItem(root, "player");
	sscanf(cJSON_GetObjectItem(obj, "position")->valuestring, "%f %f", &currentPos.x, &currentPos.y);
	currentEntity = level_entity_load(PLAYER, id++);
	currentEntity->position = currentPos;
	newLevel->player = currentEntity;

	//enemies
	obj = cJSON_GetObjectItem(root, "melts");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(ENEMY_MELT, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "milk_tanks");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(ENEMY_MILK, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "clarences");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(ENEMY_CLARENCE, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "celery_stalkers");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(ENEMY_CELERY, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "professor_slices");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(ENEMY_PROFESSOR, id++);
		currentEntity->position = currentPos;
	}

	//power_ups
	obj = cJSON_GetObjectItem(root, "heat_shields");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_SHIELD, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "goo_shots");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_GOO, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "spread_shots");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_SPREAD, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "double_taps");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_DOUBLE, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "extra_lifes");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_LIFE, id++);
		currentEntity->position = currentPos;
	}
	obj = cJSON_GetObjectItem(root, "bomb_pickups");
	entityNum = cJSON_GetObjectItem(obj, "number")->valueint;
	for(i = 0; i < entityNum; i++)
	{
		sprintf(positionString, "position%d", i);
		sscanf(cJSON_GetObjectItem(obj, positionString)->valuestring, "%f %f", &currentPos.x, &currentPos.y);
		currentEntity = level_entity_load(POWER_UP_BOMB, id++);
		currentEntity->position = currentPos;
	}

	obj = cJSON_GetObjectItem(root, "info");
	newLevel->end = cJSON_GetObjectItem(obj, "end")->valueint;
	newLevel->level_def_file = level_def_file;
	return newLevel;
}

Uint8 level_end_reached(Level *level)
{
	if(level->player->position.x > level->end)
	{
		return 1;
	}
	return 0;
}
