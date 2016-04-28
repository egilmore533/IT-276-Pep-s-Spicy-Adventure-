#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "weapon.h"
#include "camera.h"
#include "level.h"
#include "particle.h"
#include "simple_logger.h"
#include "cJSON.h"
#include "files.h"

#define PI			3.141589

Entity *weapon_fire(int type, Entity *owner)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *weapon_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Entity *bullet = NULL;
	Uint32 thinkRate;
	Vect2d vel;

	//sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	weapon_config_file = fopen(WEAPON_CONFIG,"r");
	if(!weapon_config_file)
	{
		slog("No file found: %s", WEAPON_CONFIG);
		return;
	}
	//get the length of the file
	fseek(weapon_config_file, 0, SEEK_END);
	length = ftell(weapon_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(weapon_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, weapon_config_file);
	fclose(weapon_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"weapon_config");
	if(!root)
	{
		slog("error parseing the file, file not the weapon_config");
		return;
	}

	switch(type)
	{
		case PROJECTILE_PEP_SPREAD:
			obj = cJSON_GetObjectItem(root, "pep_spread_shot");
			break;
		case PROJECTILE_PEP_CHARGE:
			obj = cJSON_GetObjectItem(root, "pep_charge_shot");
			break;
		case PROJECTILE_PEP_BOMB:
			obj = cJSON_GetObjectItem(root, "pep_bomb");
			break;
		case PROJECTILE_MELT_CREAM:
			obj = cJSON_GetObjectItem(root, "melt_cream");
			break;
		case PROJECTILE_PROFESSOR_SLICE_BREAD:
			obj = cJSON_GetObjectItem(root, "professor_slice_bread");
			break;
	}

	buf = cJSON_GetObjectItem(obj, "info");
	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;

	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;

	bullet = entity_new(thinkRate, 0, vel);
	bullet->sprite = sprite_load(filepath, frameSize, fpl, frames);
	bullet->bounds = rect(0, 0, bullet->sprite->frameSize.x, bullet->sprite->frameSize.y);
	bullet->frameNumber = frame;
	bullet->nextThink = get_time() + bullet->thinkRate;

	bullet->owner = owner;
	bullet->target = owner->target;
	bullet->think = &weapon_think;
	bullet->update = &weapon_update;
	bullet->draw = &sprite_draw;
	if(owner->bulletState == GOO_SHOT_STATE)
	{
		bullet->state = GOO_SHOT_STATE;
	}
	else
	{
		bullet->state = NORMAL_SHOT_STATE;
	}
	return bullet;
}

void weapon_think(Entity *shot)
{
	//if the bullet isn't touching the camera free the entity
	camera_free_entity_outside_bounds(shot);
}

void weapon_update(Entity *shot)
{
	vect2d_add(shot->position, shot->velocity, shot->position);
}

void weapon_pep_spread_fire(Entity *player)
{
	Entity *spice_temp;
	Vect2d velocity;
	Uint8 shots;
	int i;

	//need to set default SPREAD_SLOT value to 0 so it will fire once for 0 instead of twice for 1
	shots = player->inventory[SPREADS];
	for(i = 0; i <= shots; i++)
	{
		if(i == 0) //will always fire one shot straight forward
		{
			//this is the center bullet
			spice_temp = make_spread_bullet(player);
			vect2d_set(spice_temp->velocity, spice_temp->maxVelocity.x, 0);//will always be moving straight accross the screen horizontally
		}
		else // each extra spread shot pickup gives one extra top and bottom bullet
		{
			//top bullet
			spice_temp = make_spread_bullet(player);
			velocity.y = spice_temp->maxVelocity.y * sin(PI / 8 / i);
			velocity.x = spice_temp->maxVelocity.x * cos(PI / 8 / i);
			spice_temp->velocity = velocity;

			//bottom bullet
			spice_temp = make_spread_bullet(player);
			velocity.y *= -1;
			spice_temp->velocity = velocity;
		}
	}
}

void weapon_pep_spread_touch(Entity *spread, Entity *other)//
{
	if(other->target == spread->owner)
	{
		other->health--; // one damage for normal bullets
		if(spread->state == GOO_SHOT_STATE)
		{
			other->state = STICKIED_STATE;
		}
		spread->free(&spread);
	}
}

Entity *make_spread_bullet(Entity *owner)
{
	Entity *spread_bullet;
	spread_bullet = weapon_fire(PROJECTILE_PEP_SPREAD, owner);
	spread_bullet->think = &weapon_pep_think;
	spread_bullet->touch = &weapon_pep_spread_touch;
	spread_bullet->velocity = vect2d_new(spread_bullet->maxVelocity.x, spread_bullet->maxVelocity.y);
	vect2d_add(owner->position, vect2d_new(128, 40), spread_bullet->position);
	return spread_bullet;
}

void weapon_pep_think(Entity *spice)
{
	Vect2d offset;
	//if the bullet isn't touching the camera free the entity

	if(!(get_time() >= spice->nextThink))
	{
		return;
	}
	offset = vect2d_new(-30, -10);
	particle_exact_position_load(spice, offset);
	spice->nextThink = get_time() + spice->thinkRate;
	camera_free_entity_outside_bounds(spice);
}

void weapon_pep_charge_fire(Entity *player)
{
	Entity *spice;
	Vect2d pos, vel;
	spice = weapon_fire(PROJECTILE_PEP_CHARGE, player);
	
	spice->think = &weapon_pep_charge_think;
	spice->touch = &weapon_pep_charge_touch;
	spice->velocity = spice->maxVelocity;
	vect2d_add(player->position, vect2d_new(128, 20), spice->position);
	vect2d_add(player->position, vect2d_new(-64, 0), player->position);
}

void weapon_pep_charge_think(Entity *spice)
{
	if(!(get_time() >= spice->nextThink))
	{
		return;
	}
	particle_moving_load(spice);
	particle_moving_load(spice);
	spice->nextThink = get_time() + spice->thinkRate;
	//if the bullet isn't touching the camera free the entity
	camera_free_entity_outside_bounds(spice);
}

void weapon_pep_charge_touch(Entity *spice, Entity *other)
{
	if(other->target == spice->owner)
	{
		other->health -= 5; //five damage is fairly big considering most enemies only have 1 health
		if(spice->state == GOO_SHOT_STATE)
		{
			other->state = STICKIED_STATE;
		}
		//charge shot is so sick it blows up the baddies and keeps going looking for more baddies to tear up
	}
}

void weapon_pep_bomb(Entity *player)
{
	Entity *cam = camera_get();
	Entity *bomb = weapon_fire(PROJECTILE_PEP_BOMB, player);
	vect2d_set(bomb->position, cam->position.x, cam->position.y);
	vect2d_set(bomb->velocity, cam->velocity.x, cam->velocity.y);
	bomb->nextThink = bomb->thinkRate + get_time();
	bomb->think = &weapon_pep_bomb_think;
	bomb->touch = &weapon_pep_bomb_touch;
}

void weapon_pep_bomb_think(Entity *bomb)
{
	if(get_time() >= bomb->nextThink)
	{
		bomb->free(&bomb);
	}
}

void weapon_pep_bomb_touch(Entity *bomb, Entity *other)
{
	if(!bomb->owner)
	{
		slog("no bomb owner");
		return;
	}
	else if(!other->target)
	{
		slog("no other target");
		return;
	}
	if(other->target == bomb->owner)//if other is an enemy deplete its health, if its a power_up nothing will happen to it, but enemies will die
	{
		other->health -= 1000000; //might have to change this if I add bosses
	}
}

void weapon_melt_cream_fire(Entity *melt)
{
	Entity *cream;
	Vect2d pos, vel;
	cream = weapon_fire(PROJECTILE_MELT_CREAM, melt);
	
	cream->think = &weapon_pep_think; //temp
	cream->touch = &weapon_melt_cream_touch;
	cream->velocity = cream->maxVelocity;
	vect2d_add(melt->position, vect2d_new(0, 40), cream->position);
}

void weapon_melt_cream_touch(Entity *cream, Entity *other)
{
	if(!cream->target)
	{
		slog("no cream target");
	}
	if(other == cream->target)
	{
		other->health--;
		cream->free(&cream);
	}
}

void weapon_professor_slice_bread_fire(Entity *professor_slice)
{
	Entity *bread;
	Vect2d pos, vel;
	bread = weapon_fire(PROJECTILE_PROFESSOR_SLICE_BREAD, professor_slice);
	
	bread->think = &weapon_professor_slice_bread_think; //temp
	bread->touch = &weapon_professor_slice_bread_touch;
	bread->velocity = bread->maxVelocity;
	bread->thinkRate = rand() % 200;
	vect2d_add(professor_slice->position, vect2d_new(0, 20), bread->position);
}

void weapon_professor_slice_bread_touch(Entity *bread, Entity *other)
{
	if(!bread->target)
	{
		slog("no bread target");
		return;
	}
	if(other == bread->target)
	{
		other->health--;
		bread->free(&bread);
	}
}

void weapon_professor_slice_bread_think(Entity *bread)
{
	if(bread->velocity.x == 0)
	{
		return;
	}
	if(!(get_time() >= bread->nextThink))
	{
		return;
	}
	
	else if(bread->velocity.x < 0)
	{
		bread->velocity.x++;
		if(bread->velocity.x >= 0)
		{
			bread->velocity.x = 0;
		}
	}
	bread->nextThink = get_time() + bread->thinkRate;
	//if the bullet isn't touching the camera free the entity
	camera_free_entity_outside_bounds(bread);
}
