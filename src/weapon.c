#include <math.h>

#include "weapon.h"
#include "camera.h"
#include "level.h"
#include "simple_logger.h"

#define PI			3.141589

Entity *weapon_fire(int type, Entity *owner)
{
	Entity *bullet;
	bullet = level_entity_load(type, -1);
	bullet->owner = owner;
	bullet->think = &weapon_think;
	bullet->update = &weapon_update;
	bullet->draw = &sprite_draw;
	if(owner->state == GOO_SHOT_STATE)
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
	if(!entity_intersect(shot, camera_get()))
	{
		shot->free(&shot);
	}
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

void weapon_pep_think(Entity *spice) //this is used for spread shot and charge shot so all spiciness 
{
	Vect2d offset;
	//if the bullet isn't touching the camera free the entity
	camera_free_entity_outside_bounds(spice);

	if(!(SDL_GetTicks() >= spice->nextThink))
	{
		return;
	}
	//offset = vect2d_new(-30, -10);
	//particle_exact_position_load(spice, offset);
	spice->nextThink = SDL_GetTicks() + spice->thinkRate;

}

void weapon_pep_charge_fire(Entity *player)
{
	Entity *spice;
	Vect2d pos, vel;
	spice = weapon_fire(PROJECTILE_PEP_CHARGE, player);
	
	spice->think = &weapon_pep_think;
	spice->touch = &weapon_pep_charge_touch;
	spice->velocity = spice->maxVelocity;
	vect2d_add(player->position, vect2d_new(128, 20), spice->position);
	vect2d_add(player->position, vect2d_new(-64, 0), player->position);
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
	bomb->nextThink = bomb->thinkRate + SDL_GetTicks();
	bomb->think = &weapon_pep_bomb_think;
	bomb->touch = &weapon_pep_bomb_touch;
}

void weapon_pep_bomb_think(Entity *bomb)
{
	if(SDL_GetTicks() >= bomb->nextThink)
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
