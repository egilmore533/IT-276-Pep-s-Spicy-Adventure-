#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "camera.h"
#include "cJSON.h"
#include "simple_logger.h"

static Entity	*player = NULL;
static Uint32	respawn_moment;
static Uint8	lives = 3;
static Uint8	bombs = 2;
static Uint8	spreads = 1;
static Vect2d	velocity_moving;

void player_load(Vect2d position, int id)
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *player_config_file;
	long length;
	char *data;
	char *string;

	//entity info
	Vect2d pos = position; 
	Vect2d vel = position; //needs to be initialized, doesn't matter for now so re use position
	Uint32 nextThink, thinkRate;
	int health, type;

	//sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	player_config_file = fopen("def/player_config.txt","r");
	if(!player_config_file)
	{
		slog("No file found: %s", "def/player_config.txt");
		return;
	}
	//get the length of the file
	fseek(player_config_file, 0, SEEK_END);
	length = ftell(player_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(player_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, player_config_file);
	fclose(player_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"player_config");
	if(!root)
	{
		slog("error parseing the file, file not the player_config");
		return;
	}

	obj = cJSON_GetObjectItem(root, "player");

	buf = cJSON_GetObjectItem(obj, "info");

	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "vel")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	nextThink = cJSON_GetObjectItem(buf, "nextThink")->valueint;

	buf = cJSON_GetObjectItem(obj, "sprite");
	filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
	sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
	frame = cJSON_GetObjectItem(buf, "frame")->valueint;
	fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
	frames = cJSON_GetObjectItem(buf, "frames")->valueint;

	player = entity_new(nextThink, thinkRate, health, pos, vel);
	player->sprite = sprite_load(filepath, frameSize, fpl, frames);
	player->frameNumber = frame;
	player->bounds = rect(player->position.x, position.y, frameSize.x, frameSize.y);
	player->update = &player_update;
	player->think = &player_think;
	player->draw = &sprite_draw;
	player->free = &player_free;

	player->owner = camera_get();
	velocity_moving = vect2d_new(vel.x, vel.y);
}

void player_think(Entity *player)
{
	static Uint32 full_charge;
	const Uint8 *keys;
	SDL_Event click_event;
	static Uint8 clicked = 0;
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
				player->nextThink = SDL_GetTicks() + player->thinkRate;
			}
		}
		else
		{
			if(SDL_GetTicks() >= player->nextThink)
			{
				//weapon_pep_charge_fire(player);
				slog("charged Fire");
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
		if(lives == 0)
		{
			//game over code here
			player->free(player);
			camera_stop();
			return;
		}
		lives--;
		tempID = player->id;
		tempPos = player->position;
		player->free(player);
		respawn_moment = SDL_GetTicks() + RESPAWN_RATE;
		player_death(tempID, tempPos);
		return;
	}
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_A])
	{
		if(!(player->position.x <= player->owner->position.x))
		{
			player->velocity.x = -velocity_moving.x;
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
			player->velocity.x = velocity_moving.x;
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
			player->velocity.y = -velocity_moving.y;
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
			player->velocity.y = velocity_moving.y;
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

void player_death(int id, Vect2d pos)
{
	Entity *new_player = NULL;
	while(1)
	{
		if(SDL_GetTicks() >= respawn_moment)
		{
			player_load(vect2d_new(camera_get()->position.x, pos.y), id); //spawns the player up against the camera's leftmost wall at the y point they were on when they died
			return;
		}
	}
}

void player_add_life()
{
	lives++;
}

Entity *player_get()
{
	return player;
}
