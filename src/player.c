#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "weapon.h"
#include "camera.h"
#include "cJSON.h"
#include "simple_logger.h"
#include "audio.h"
#include "files.h"

static Entity	*player = NULL;
static Uint8	player_saved_load = 0;
static Uint32	respawn_moment;

Entity *player_load()
{
	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *player_config_file;
	long length;
	char *data;
	char *def_file_path;

	//entity info
	Uint32 thinkRate;
	Vect2d vel;
	int health;
	int lives, bombs, spreads;

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

	if(!player_saved_load)
	{
		def_file_path = PLAYER_CONFIG;
	}
	else
	{
		def_file_path = PLAYER_SAVE_CONFIG;
	}
	player_config_file = fopen(def_file_path,"r");
	if(!player_config_file)
	{
		slog("No file found: %s", PLAYER_CONFIG);
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
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	lives = cJSON_GetObjectItem(buf, "lives")->valueint;
	bombs = cJSON_GetObjectItem(buf, "bombs")->valueint;
	spreads = cJSON_GetObjectItem(buf, "spreads")->valueint;

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

	player = entity_new(thinkRate, health, vel);
	player->sprite = sprite_load(filepath, frameSize, fpl, frames);
	player->bounds = rect(0, 0, player->sprite->frameSize.x, player->sprite->frameSize.y);
	player->entitySounds = audio_load_pak(channel, name, fire1_file, fire2_file, death_file, moving_file);
	player->frameNumber = frame;
	player->nextThink = get_time() + player->thinkRate;

	player->update = &player_update;
	player->think = &player_think;
	player->draw = &sprite_draw;

	player->inventory[LIVES] = lives;
	player->inventory[BOMBS] = bombs;
	player->inventory[SPREADS] = spreads;

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
			player->state = GAME_OVER_STATE;
			player_saved_load_off();
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
	/////////////////test//////////////////
	if(keys[SDL_SCANCODE_Q])
	{
		player->position.x = 20000;
	}
	////////////////////////////////
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

void player_save_info()
{
	FILE *player_save_config_file;
	cJSON *json, *root, 
		*obj,  *buf;
	char *data;
	char temp[100];
	int i;

	if(!player_saved_load)
	{
		return;
	}
	if(player == NULL)
	{
		slog("cannot save player data if player not loaded");
		return;
	}
	json = cJSON_CreateObject();
	cJSON_AddItemToObject(json, "player_config", root=cJSON_CreateObject());
	cJSON_AddItemToObject(root, "player", obj = cJSON_CreateObject());

	cJSON_AddItemToObject(obj, "sprite", buf = cJSON_CreateObject());
	cJSON_AddStringToObject(buf, "file", player->sprite->filename);
	memset(&temp[0], 0, sizeof(temp));
	sprintf(temp, "%d %d", player->bounds.w, player->bounds.h);
	cJSON_AddStringToObject(buf, "bounds", temp);
	cJSON_AddNumberToObject(buf, "frame", player->frameNumber);
	cJSON_AddNumberToObject(buf, "fpl", player->sprite->fpl);
	cJSON_AddNumberToObject(buf, "frames", player->sprite->frames);

	cJSON_AddItemToObject(obj, "info", buf = cJSON_CreateObject());
	cJSON_AddNumberToObject(buf, "thinkRate", player->thinkRate);
	cJSON_AddNumberToObject(buf, "health", player->health);
	memset(&temp[0], 0, sizeof(temp));
	sprintf(temp, "%f %f", player->maxVelocity.x, player->maxVelocity.y);
	cJSON_AddStringToObject(buf, "velMax", temp);
	cJSON_AddNumberToObject(buf, "lives", player->inventory[LIVES]);
	cJSON_AddNumberToObject(buf, "bombs", player->inventory[BOMBS]);
	cJSON_AddNumberToObject(buf, "spreads", player->inventory[SPREADS]);

	cJSON_AddItemToObject(obj, "audioPak", buf = cJSON_CreateObject());
	cJSON_AddStringToObject(buf, "name", player->entitySounds->name);
	cJSON_AddStringToObject(buf, "firing1", player->entitySounds->firing1->filename);
	cJSON_AddStringToObject(buf, "firing2", player->entitySounds->firing2->filename);
	cJSON_AddStringToObject(buf, "death", player->entitySounds->death->filename);
	cJSON_AddStringToObject(buf, "moving", player->entitySounds->moving->filename);

	data = cJSON_Print(json);
	cJSON_Delete(json);

	player_save_config_file = fopen(PLAYER_SAVE_CONFIG, "w");

	if (player_save_config_file == NULL) {
	  fprintf(stderr, "Can't open output file %s!\n",
			  PLAYER_SAVE_CONFIG);
	  return;
	}

	fprintf(player_save_config_file, data);

	fclose(player_save_config_file);
}

void player_add_life()
{
	player->inventory[LIVES]++;
}

Entity *player_get()
{
	return player;
}

void player_saved_load_on()
{
	player_saved_load = 1;
}

void player_saved_load_off()
{
	player_saved_load = 0;
}

