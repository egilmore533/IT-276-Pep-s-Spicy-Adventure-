#include <stdlib.h>
#include <math.h>

#include "enemy.h"
#include "weapon.h"
#include "camera.h"
#include "particle.h"
#include "weapon.h"
#include "simple_logger.h"
#include "player.h"


/////////////////////////////CELERY STALKER CODE////////////////////////////////////

Entity *celery_stalker_load(Entity *celery_stalker)
{
	celery_stalker->update = &celery_stalker_update;
	celery_stalker->touch = &celerly_stalker_touch;
	celery_stalker->draw = &sprite_draw;
	celery_stalker->state = NORMAL_STATE;
	celery_stalker->target = player_get();
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
			celery_stalker->thinkRate = 2000;
			celery_stalker->nextThink = get_time() + celery_stalker->thinkRate;
		}
	}
	else if(other == celery_stalker->target)
	{
		other->health--;
		celery_stalker->free(&celery_stalker);
	}
}

//////////////////////////////////CLARENCE CODE////////////////////////////////////////

Entity *clarence_load(Entity *clarence)
{
	clarence->touch = &clarence_touch;
	clarence->update = &clarence_update;
	clarence->draw = &sprite_draw;
	clarence->state = NORMAL_STATE;
	clarence->target = player_get();
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
		slog("\n\n hit player \n\n");
		other->health--;
		clarence->free(&clarence);
	}
}

////////////////////////////////MELT CODE////////////////////////////////////

Entity *melt_load(Entity *melt)
{
	melt->touch = &melt_touch;
	melt->update = &melt_update;
	melt->draw = &sprite_draw;
	melt->state = NORMAL_STATE;
	melt->target = player_get();
	return melt;
}

void melt_think(Entity *melt)
{
	if(!melt->target)
	{
		slog("no melt target");
		return;
	}
	camera_free_entity_outside_bounds(melt);
	melt->velocity.y = melt->maxVelocity.y * sin((float)get_time());
	melt->velocity.x = melt->velocity.x;
	vect2d_negate(melt->velocity, melt->velocity);
	if(!(get_time() >= melt->nextThink))
	{
		return;
	}
	weapon_melt_cream_fire(melt);
	melt->nextThink = get_time() + melt->thinkRate;
}

void melt_update(Entity *melt)
{
	if(melt->think)
	{
		vect2d_add(melt->position, melt->velocity, melt->position);
		vect2d_copy(melt->velocity, melt->maxVelocity);
	}
	entity_intersect_all(melt);
	if(melt->health <= 0)
	{
		particle_bundle_load(melt, 40);
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
		melt->free(&melt);
	}
}

/////////////////////////////MILK TANK CODE///////////////////////////////////////

Entity *milk_tank_load(Entity *milk_tank)
{
	milk_tank->touch = &milk_tank_touch;
	milk_tank->update = &milk_tank_update;
	milk_tank->target = player_get();
	milk_tank->draw = &sprite_draw;
	milk_tank->state = NORMAL_STATE;
	milk_tank->target = player_get();
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
		milk_tank->thinkRate = 2000;
		milk_tank->nextThink = get_time() + milk_tank->thinkRate;
		return;
	}
	if(!(get_time() >= milk_tank->nextThink))
	{
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
	milk_tank->thinkRate = 40;
	milk_tank->nextThink = milk_tank->thinkRate + get_time();
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
	professor_slice->target = player_get();
	professor_slice->update = &professor_slice_update;
	professor_slice->touch = &professor_slice_touch;
	professor_slice->draw = &sprite_draw;
	professor_slice->state = NORMAL_STATE;
	professor_slice->target = player_get();
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
	if(!(get_time() >= professor_slice->nextThink))
	{
		return;
	}
	vect2d_copy(professor_slice->velocity, professor_slice->maxVelocity);
	camera_free_entity_outside_bounds(professor_slice);
	vect2d_subtract(professor_slice->target->position, professor_slice->position, professor_slice->direction);
	vect2d_normalize(&professor_slice->direction);
	vect2d_negate(professor_slice->direction, professor_slice->direction);
	vect2d_mutiply(professor_slice->velocity, professor_slice->direction, professor_slice->velocity);
	weapon_professor_slice_bread_fire(professor_slice);
	professor_slice->nextThink = get_time() + professor_slice->thinkRate;
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

		if(professor_slice->position.y + professor_slice->sprite->frameSize.y >= cam->bounds.x + cam->bounds.h)
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
		professor_slice->free(&professor_slice);
	}
}