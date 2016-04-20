#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "camera.h"
#include "cJSON.h"
#include "simple_logger.h"
#include "audio.h"

static Entity	*player = NULL;
static Uint32	respawn_moment;
static Uint8	lives = 3;
static Uint8	bombs = 2;
static Uint8	spreads = 1;

Entity *player_load(Entity *player, Vect2d position, int id)
{
	player->bounds = rect(player->position.x, position.y, player->sprite->frameSize.x, player->sprite->frameSize.y);
	player->update = &player_update;
	player->think = &player_think;
	player->draw = &sprite_draw;

	player->owner = camera_get();
	player->maxVelocity = vect2d_new(player->velocity.x, player->velocity.y);
}

void player_think(Entity *player)
{
	static Uint32 full_charge;
	const Uint8 *keys;
	SDL_Event click_event;
	static Uint8 clicked = 0;
	if(SDL_GetTicks() < respawn_moment)
	{
		return;
	}
	SDL_PollEvent(&click_event);
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_SPACE])
	{
		if(SDL_GetTicks() >= player->nextThink)
		{
			if(bombs > 0)
			{
				//weapon_pep_bomb(player);
				slog("bomb fire");
				bombs--;
				player->nextThink = SDL_GetTicks() + player->thinkRate;
			}
		}
	}
	if(click_event.type == SDL_MOUSEBUTTONDOWN)
	{
		clicked = 1;
		full_charge = SDL_GetTicks() + CHARGE_RATE;
	}
	else if(click_event.type == SDL_MOUSEBUTTONUP && clicked)
	{
		if(full_charge >= SDL_GetTicks())
		{
			if(SDL_GetTicks() >= player->nextThink)
			{
				//weapon_pep_spread_fire(player);
				slog("spread fire");
				audio_play_sound(player->entitySounds->firing1);
				player->nextThink = SDL_GetTicks() + player->thinkRate;
			}
		}
		else
		{
			if(SDL_GetTicks() >= player->nextThink)
			{
				//weapon_pep_charge_fire(player);
				slog("charged Fire");
				audio_play_sound(player->entitySounds->firing2);
				player->nextThink = SDL_GetTicks() + player->thinkRate;
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
	if(SDL_GetTicks() < respawn_moment)
	{
		return;
	}
	//if the player picked up the shield power up it will get a health of 2
	//if STATE isn't SHIELDED, but the player health is 2 then the shield needs to be activated
	if(player->health > 1 && player->state != SHIELDED_STATE)
	{
		player->state = SHIELDED_STATE;
		player->frameNumber++;
	}
	//if the player has a SHIELD, but has a health of 1 then they lost their shield
	//set the state to NORMAL and reset the sprite to be the normal player sprite
	else if(player->state == SHIELDED_STATE && player->health == 1)
	{
		player->state = NORMAL_STATE;
		player->frameNumber--;
	}
	//if the player's health is less than 1 then kill
	else if(player->health <= 0)
	{
		audio_play_sound(player->entitySounds->death);
		if(lives == 0)
		{
			//game over code here
			player->free(&player);
			camera_stop();
			return;
		}
		lives--;
		player->health = 100000;
		player->frameNumber++;
		respawn_moment = SDL_GetTicks() + 500;
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

void player_free(Entity *player)
{
	entity_free(&player);
}

void player_add_life()
{
	lives++;
}

Entity *player_get()
{
	return player;
}