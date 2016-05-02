#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "graphics.h"
#include "enemy.h"
#include "weapon.h"
#include "camera.h"
#include "particle.h"
#include "weapon.h"
#include "simple_logger.h"
#include "cJSON.h"
#include "player.h"
#include "files.h"

Entity *enemy_load(int type, Entity *enemy)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *enemy_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Uint32 thinkRate;
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
	int channel = FX_Enemy;

	enemy_config_file = fopen(ENEMY_CONFIG,"r");
	if(!enemy_config_file)
	{
		slog("No file found: %s", ENEMY_CONFIG);
		return;
	}
	//get the length of the file
	fseek(enemy_config_file, 0, SEEK_END);
	length = ftell(enemy_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(enemy_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, enemy_config_file);
	fclose(enemy_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"enemy_config");
	if(!root)
	{
		slog("error parseing the file, file not the enemy_config");
		return;
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
	}

	buf = cJSON_GetObjectItem(obj, "info");
	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;

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

	enemy = entity_new(thinkRate, health, vel);
	enemy->sprite = sprite_load(filepath, frameSize, fpl, frames);
	enemy->bounds = rect(0, 0, enemy->sprite->frameSize.x, enemy->sprite->frameSize.y);
	enemy->entitySounds = audio_load_pak(channel, name, fire1_file, fire2_file, death_file, moving_file);
	enemy->frameNumber = frame;
	enemy->nextThink = get_time() + enemy->thinkRate;

	switch(type)
	{
	case ENEMY_CLARENCE:
		enemy = clarence_load(enemy);
		break;
	case ENEMY_MELT:
		enemy = melt_load(enemy);
		break;
	case ENEMY_MILK:
		enemy = milk_tank_load(enemy);
		break;
	case ENEMY_CELERY:
		enemy = celery_stalker_load(enemy);
		break;
	case ENEMY_PROFESSOR:
		enemy = professor_slice_load(enemy);
		break;
	}

	enemy->draw = &sprite_draw;
	enemy->free = &entity_free;
	enemy->state = NORMAL_STATE;
	enemy->target = player_get();
	return enemy;
}


/////////////////////////////CELERY STALKER CODE////////////////////////////////////

Entity *celery_stalker_load(Entity *celery_stalker)
{
	celery_stalker->update = &celery_stalker_update;
	celery_stalker->touch = &celerly_stalker_touch;
	return celery_stalker;
}

void celery_stalker_think_start(Entity *celery_stalker)
{
	if(!celery_stalker->target)
	{
		slog("No celery_stalker target");
		return;
	}
	if(get_time() >= celery_stalker->nextThink)
	{
		vect2d_subtract(celery_stalker->target->position, celery_stalker->position, celery_stalker->direction);
		vect2d_normalize(&celery_stalker->direction);
		celery_stalker->think = &celery_stalker_think_moving;
		vect2d_copy(celery_stalker->velocity, celery_stalker->maxVelocity);
		vect2d_mutiply(celery_stalker->velocity, celery_stalker->direction, celery_stalker->velocity);
	}
}

void celery_stalker_think_moving(Entity *celery_stalker)
{
	//kill the celery if it has left the camera's bounds
	camera_free_entity_outside_bounds(celery_stalker);
}

void celery_stalker_update(Entity *celery_stalker)
{
	//this checks if the celery_stalker has already took aim  on the player
	if(celery_stalker->think == &celery_stalker_think_moving)
	{
		vect2d_add(celery_stalker->position, celery_stalker->velocity, celery_stalker->position);
	}
	entity_intersect_all(celery_stalker);
	if(celery_stalker->health <= 0)
	{
		particle_bundle_load(celery_stalker, 20);
		audio_play_sound(celery_stalker->entitySounds->death);
		celery_stalker->free(&celery_stalker);
	}
}

void celerly_stalker_touch(Entity *celery_stalker, Entity *other)
{
	if(!celery_stalker->target)
	{
		slog("No celery_stalker target");
		return;
	}
	if(!celery_stalker->think)
	{
		if(other == camera_get())
		{
			celery_stalker->think = &celery_stalker_think_start;
			celery_stalker->nextThink = get_time() + celery_stalker->thinkRate;
		}
	}
	else if(other == celery_stalker->target)
	{
		other->health--;
		audio_play_sound(celery_stalker->entitySounds->death);
		celery_stalker->free(&celery_stalker);
	}
}

//////////////////////////////////CLARENCE CODE////////////////////////////////////////

Entity *clarence_load(Entity *clarence)
{
	clarence->touch = &clarence_touch;
	clarence->update = &clarence_update;
	return clarence;
}

void clarence_think_start(Entity *clarence)
{
	Vect2d dir = vect2d_new(0, 0);
	Vect2d vel = vect2d_new(clarence->maxVelocity.x, clarence->maxVelocity.y);

	if(!clarence->target)
	{
		slog("no clarence target");
		return;
	}
	if(clarence->target->position.x + clarence->target->sprite->frameSize.x * 0.5 >= clarence->position.x)
	{
		dir = vect2d_new(0, 1);
		clarence->think = &clarence_think_moving;
		clarence->direction = dir;
		clarence->velocity = vel;
		vect2d_mutiply(clarence->velocity, clarence->direction, clarence->velocity);
		clarence->health = 1;
	}
	else
	{
		clarence->direction = dir;
		vect2d_mutiply(clarence->velocity, clarence->direction, clarence->velocity);
	}
}

void clarence_think_moving(Entity *clarence)
{
	camera_free_entity_outside_bounds(clarence);
}

void clarence_update(Entity *clarence)
{
	if(clarence->health <= 0)
	{
		clarence->free(&clarence);
	}
	vect2d_add(clarence->position, clarence->velocity, clarence->position);
	entity_intersect_all(clarence);
	if(clarence->health <= 0)
	{
		particle_bundle_load(clarence, 40);
		audio_play_sound(clarence->entitySounds->death);
		clarence->free(&clarence);
	}
}

void clarence_touch(Entity *clarence, Entity *other)
{
	if(!clarence->target)
	{
		slog("no clarence target");
		return;
	}
	if(!clarence->think)
	{
		if(entity_intersect(clarence, camera_get()))
		{
			clarence->think = &clarence_think_start;
		}
	}
	if(other == clarence->target)
	{
		other->health--;
		audio_play_sound(clarence->entitySounds->death);
		clarence->free(&clarence);
	}
}

////////////////////////////////MELT CODE////////////////////////////////////

Entity *melt_load(Entity *melt)
{
	melt->touch = &melt_touch;
	melt->update = &melt_update;
	return melt;
}

void melt_think(Entity *melt)
{
	if(!melt->target)
	{
		slog("no melt target");
		return;
	}
	if(melt->state == STICKIED_STATE)
	{
		melt->think = &melt_stickied_think;
		vect2d_set(melt->velocity, 0, 0);
		melt->nextThink = get_time() + 2000;
		return;
	}
	melt->velocity.y = melt->maxVelocity.y * sin(get_time() * 2 * 3.14 / 2000);
	melt->velocity.x = melt->maxVelocity.x;
	vect2d_negate(melt->velocity, melt->velocity);
	if(!(get_time() >= melt->nextThink))
	{
		return;
	}
	weapon_melt_cream_fire(melt);
	melt->nextThink = get_time() + melt->thinkRate;
	camera_free_entity_outside_bounds(melt);
}

void melt_stickied_think(Entity *melt)
{
	vect2d_set(melt->velocity, 0, 0);
	if(!(get_time() >= melt->nextThink))
	{
		return;
	}
	melt->state = NORMAL_STATE;
	melt->think = &melt_think;
	melt->nextThink = melt->thinkRate + get_time();
}

void melt_update(Entity *melt)
{
	Entity *cam = camera_get();
	if(melt->think)
	{
		if(melt->position.y <= cam->bounds.x)
		{
			if(melt->velocity.y < 0)
			{
				melt->velocity.y = 0;
			}
		}

		if(melt->position.y + melt->sprite->frameSize.y >= cam->position.y + HUD_HEIGHT)
		{
			if(melt->velocity.y > 0)
			{
				melt->velocity.y = 0;
			}
		}
		vect2d_add(melt->position, melt->velocity, melt->position);
	}
	entity_intersect_all(melt);
	if(melt->health <= 0)
	{
		particle_bundle_load(melt, 40);
		audio_play_sound(melt->entitySounds->death);
		melt->free(&melt);
	}
}

void melt_touch(Entity *melt, Entity *other)
{
	if(!melt->target)
	{
		slog("no melt target");
		return;
	}
	if(!melt->think)
	{
		if(other == camera_get())
		{
			melt->think = &melt_think;
		}
	}
	else if(other == melt->target)
	{
		other->health--;
		audio_play_sound(melt->entitySounds->death);
		melt->free(&melt);
	}
}

/////////////////////////////MILK TANK CODE///////////////////////////////////////

Entity *milk_tank_load(Entity *milk_tank)
{
	milk_tank->touch = &milk_tank_touch;
	milk_tank->update = &milk_tank_update;
	return milk_tank;
}

void milk_tank_think(Entity *milk_tank)
{
	if(!milk_tank->target)
	{
		slog("no milk_tank");
		return;
	}
	if(milk_tank->state == STICKIED_STATE)
	{
		milk_tank->think = &milk_tank_stickied_think;
		vect2d_set(milk_tank->velocity, 0, 0);
		milk_tank->nextThink = get_time() + 2000;
		return;
	}
	vect2d_subtract(milk_tank->target->position, milk_tank->position, milk_tank->direction);
	vect2d_normalize(&milk_tank->direction);
	vect2d_mutiply(milk_tank->velocity, milk_tank->direction, milk_tank->velocity);
	//no camera free because pep is locked to the screen, and milk_tank will always go towards pep
}

void milk_tank_stickied_think(Entity *milk_tank)
{
	vect2d_set(milk_tank->velocity, 0, 0);
	if(!(get_time() >= milk_tank->nextThink))
	{
		return;
	}
	milk_tank->state = NORMAL_STATE;
	milk_tank->think = &milk_tank_think;
}

void milk_tank_update(Entity *milk_tank)
{
	if(milk_tank->think)
	{
		vect2d_add(milk_tank->position, milk_tank->velocity, milk_tank->position);
		vect2d_copy(milk_tank->velocity, milk_tank->maxVelocity);
	}
	entity_intersect_all(milk_tank);
	if(milk_tank->health <= 0)
	{
		particle_bundle_load(milk_tank, 40);
		audio_play_sound(milk_tank->entitySounds->death);
		milk_tank->free(&milk_tank);
	}
	
}

void milk_tank_touch(Entity *milk_tank, Entity *other)
{
	if(!milk_tank->target)
	{
		slog("no milk_tank target");
		return;
	}
	if(other == milk_tank->target)
	{
		other->health--;
		audio_play_sound(milk_tank->entitySounds->death);
		milk_tank->free(&milk_tank);
	}
	else if(!milk_tank->think && other == camera_get())
	{
		milk_tank->think = &milk_tank_think;
	}
}

//////////////////////////////PROFESSOR SLICE CODE//////////////////////////////////

Entity *professor_slice_load(Entity *professor_slice)
{
	professor_slice->update = &professor_slice_update;
	professor_slice->touch = &professor_slice_touch;
	return professor_slice;
}

void professor_slice_think(Entity *professor_slice)
{
	if(!professor_slice->target)
	{
		slog("no professor slice target");
		return;
	}
	if(professor_slice->state == STICKIED_STATE)
	{
		professor_slice->think = &professor_slice_stickied_think;
		vect2d_set(professor_slice->velocity, 0, 0);
		professor_slice->nextThink = get_time() + professor_slice->thinkRate;
		return;
	}

	vect2d_copy(professor_slice->velocity, professor_slice->maxVelocity);

	vect2d_subtract(professor_slice->target->position, professor_slice->position, professor_slice->direction);
	vect2d_normalize(&professor_slice->direction);
	vect2d_negate(professor_slice->direction, professor_slice->direction);
	vect2d_mutiply(professor_slice->velocity, professor_slice->direction, professor_slice->velocity);
	if(!(get_time() >= professor_slice->nextThink))
	{
		return;
	}
	weapon_professor_slice_bread_fire(professor_slice);
	professor_slice->nextThink = get_time() + professor_slice->thinkRate;
	camera_free_entity_outside_bounds(professor_slice);
}

void professor_slice_stickied_think(Entity *professor_slice)
{
	vect2d_set(professor_slice->velocity, 0, 0);
	if(!(get_time() >= professor_slice->nextThink))
	{
		return;
	}
	professor_slice->state = NORMAL_STATE;
	professor_slice->think = &professor_slice_think;
}

void professor_slice_update(Entity *professor_slice)
{
	Entity *cam = camera_get();
	if(!cam)
	{
		return;
	}

	if(professor_slice->think)
	{
		if(professor_slice->position.y <= cam->bounds.x)
		{
			if(professor_slice->velocity.y < 0)
			{
				professor_slice->velocity.y = rand() % 2;
			}
		}

		if(professor_slice->position.y + professor_slice->sprite->frameSize.y >= cam->position.y + HUD_HEIGHT)
		{
			if(professor_slice->velocity.y > 0)
			{
				professor_slice->velocity.y = -(rand() % 2);
			}
		}
		vect2d_add(professor_slice->position, professor_slice->velocity, professor_slice->position);
	}
	entity_intersect_all(professor_slice);
	if(professor_slice->health <= 0)
	{
		particle_bundle_load(professor_slice, 40);
		audio_play_sound(professor_slice->entitySounds->death);
		professor_slice->free(&professor_slice);
	}
}

void professor_slice_touch(Entity *professor_slice, Entity *other)
{
	if(!professor_slice->target)
	{
		slog("no professor slice target");
		return;
	}
	
	if(!professor_slice->think)
	{
		if(other == camera_get())
		{
			professor_slice->think = &professor_slice_think;
		}
	}
	else if(other == professor_slice->target)
	{
		other->health--;
		audio_play_sound(professor_slice->entitySounds->death);
		professor_slice->free(&professor_slice);
	}
}