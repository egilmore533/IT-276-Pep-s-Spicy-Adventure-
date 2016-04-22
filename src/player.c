#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "weapon.h"
#include "camera.h"
#include "cJSON.h"
#include "simple_logger.h"
#include "audio.h"

static Entity	*player = NULL;
static Uint32	respawn_moment;

Entity *player_load(Entity *p)
{
	player = p;
	player->update = &player_update;
	player->think = &player_think;
	player->draw = &sprite_draw;

	player->owner = camera_get();
	return player;
}

void player_think(Entity *player)
{
	static Uint32 full_charge;
	const Uint8 *keys;
	SDL_Event click_event;
	static Uint8 clicked = 0;
	if(get_time() > respawn_moment && player->health == 3 )
	{
		player->health = 1;
		player->frameNumber= 0;
	}
	SDL_PollEvent(&click_event);
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_SPACE])
	{
		if(get_time() >= player->nextThink)
		{
			if(player->inventory[BOMBS] > 0)
			{
				audio_play_sound(player->entitySounds->moving); //using moving here for an extra firing sound
				weapon_pep_bomb(player);
				player->inventory[BOMBS]--;
				player->nextThink = get_time() + player->thinkRate;
			}
		}
	}
	if(click_event.type == SDL_MOUSEBUTTONDOWN)
	{
		clicked = 1;
		full_charge = get_time() + CHARGE_RATE;
	}
	else if(click_event.type == SDL_MOUSEBUTTONUP && clicked)
	{
		if(full_charge >= get_time())
		{
			if(get_time() >= player->nextThink)
			{
				weapon_pep_spread_fire(player);
				audio_play_sound(player->entitySounds->firing1);
				player->nextThink = get_time() + player->thinkRate;
			}
		}
		else
		{
			if(get_time() >= player->nextThink)
			{
				weapon_pep_charge_fire(player);
				audio_play_sound(player->entitySounds->firing2);
				player->nextThink = get_time() + player->thinkRate;
			}
		}
		clicked = 0;
	}
}

void player_update(Entity *player)
{
	int tempID;
	Vect2d tempPos;
	const Uint8 *keys;
	if(!player->owner)
	{
		slog("player has no owner");
		return;
	}
	if(get_time() < respawn_moment && player->health != 3)
	{
		player->health = 3;
		player->frameNumber= 2;
	}
	//if the player picked up the shield power up it will get a health of 2
	//if STATE isn't SHIELDED, but the player health is 2 then the shield needs to be activated
	if(player->health == 2 && player->state != SHIELDED_STATE)
	{
		player->state = SHIELDED_STATE;
		player->frameNumber = 1;
	}
	//if the player has a SHIELD, but has a health of 1 then they lost their shield
	//set the state to NORMAL and reset the sprite to be the normal player sprite
	else if(player->state == SHIELDED_STATE && player->health == 1)
	{
		player->state = NORMAL_STATE;
		player->frameNumber = 0;
	}
	//if the player's health is less than 1 then kill
	else if(player->health <= 0)
	{
		audio_play_sound(player->entitySounds->death);
		if(player->inventory[LIVES] == 0)
		{
			//game over code here
			player->free(&player);
			camera_stop();
			return;
		}
		player->inventory[LIVES]--;
		player->health = 3;
		player->frameNumber = 2;
		respawn_moment = get_time() + RESPAWN_RATE;
		return;
	}
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_Q])
	{
		player->health--;
	}
	if(keys[SDL_SCANCODE_A])
	{
		if(!(player->position.x <= player->owner->position.x))
		{
			player->velocity.x = -player->maxVelocity.x;
		}
		else
		{
			player->velocity.x = 0;
		}
	}
	else if(keys[SDL_SCANCODE_D])
	{
		if(!(player->position.x + player->sprite->frameSize.x >= player->owner->position.x + player->owner->bounds.w))
		{
			player->velocity.x = player->maxVelocity.x;
		}
		else
		{
			player->velocity.x = 0;
		}
	}
	else
	{
		player->velocity.x = 0;
	}
	if(keys[SDL_SCANCODE_W])
	{
		if(!player->position.y <= player->owner->bounds.y) //this keeps pep from moving above camera, can assume camera's y position is 0
		{
			player->velocity.y = -player->maxVelocity.y;
		}
		else
		{
			player->velocity.y = 0;
		}
	}
	else if(keys[SDL_SCANCODE_S])
	{
		//add player's sprite height to make it perfect
		if(!(player->position.y + player->sprite->frameSize.y > player->owner->position.y + player->owner->bounds.h)) // keeps pep from moving below camera, adds camera's y (0) to its height to get the position of the bottom of the camera
		{
			player->velocity.y = player->maxVelocity.y;
		}
		else
		{
			player->velocity.y = 0;
		}
	}
	else
	{
		player->velocity.y = 0;
	}

	vect2d_add(player->position, player->velocity, player->position);
	if(player->position.x < player->owner->position.x)
	{
		player->position.x = player->owner->position.x;
	}
	entity_intersect_all(player);
}

void player_add_life()
{
	player->inventory[LIVES]++;
}

Entity *player_get()
{
	return player;
}
