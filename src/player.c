#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"
#include "simple_logger.h"

#include "player.h"
#include "audio.h"
#include "camera.h"
#include "files.h"
#include "mouse.h"
#include "weapon.h"


/* player resource manager */
static Entity	*player = NULL;
static Uint8	playerSavedLoad = 0;

/* player respawning data */
static Uint32	respawnMoment;
static Uint32	thinkRateMin = 200;

/* shield sprite */
static Sprite	*shield = NULL;

/* points data */
static int		lastPointReward = 0;
static int		pointRewardRate = 0;

/* multiplier data */
static Uint8	multiplier = 0;
static Uint8	multiplierMax = 0;
static Uint8	multiplierMin = 1;

/* mutiplier degrade data */
static Uint32	mutiplierDegradeTime = 0;
static Uint32	multiplierDegradeRate = 0;
static Uint32	multiplierDegradeMin = 0;
static Uint32	multiplierDegradeMax = 0;

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
	Uint32 points;
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
	int channel = FX_PLAYER;

	if(!playerSavedLoad)
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
		return NULL;
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
		return NULL;
	}
	obj = cJSON_GetObjectItem(root, "player");
	buf = cJSON_GetObjectItem(obj, "info");
	//reads string that is two floats and sets them to be the two components of vel
	sscanf(cJSON_GetObjectItem(buf, "velMax")->valuestring, "%f %f", &vel.x, &vel.y);
	health = cJSON_GetObjectItem(buf, "health")->valueint;
	thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
	points = cJSON_GetObjectItem(buf, "points")->valueint;
	lives = cJSON_GetObjectItem(buf, "lives")->valueint;
	bombs = cJSON_GetObjectItem(buf, "bombs")->valueint;
	spreads = cJSON_GetObjectItem(buf, "spreads")->valueint;

	pointRewardRate = cJSON_GetObjectItem(buf, "pointRewardRate")->valueint;
	lastPointReward = cJSON_GetObjectItem(buf, "lastPointReward")->valueint;

	multiplier = cJSON_GetObjectItem(buf, "multiplier")->valueint;
	multiplierMax = cJSON_GetObjectItem(buf, "multiplierMax")->valueint;
	multiplierDegradeRate = cJSON_GetObjectItem(buf, "multiplierDegradeRate")->valueint;
	multiplierDegradeMin = cJSON_GetObjectItem(buf, "multiplierDegradeMin")->valueint;
	multiplierDegradeMax = cJSON_GetObjectItem(buf, "multiplierDegradeMax")->valueint;

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

	player->points = points;
	player->inventory[LIVES] = lives;
	player->inventory[BOMBS] = bombs;
	player->inventory[SPREADS] = spreads;

	player->owner = camera_get();

	if(player->health > 1)
	{
		player->state = SHIELDED_STATE;
	}

	//shield bloom effect stuff
	shield = sprite_load("images/ball.png", vect2d_new(144, 144), 1, 1);
	SDL_SetTextureBlendMode(shield->image, SDL_BLENDMODE_ADD);
	return player;
}

void player_think(Entity *player)
{
	Mouse *mouse = mouse_get();
	static Uint32 full_charge;
	const Uint8 *keys;
	SDL_Event click_event;
	static Uint8 clicked = 0;
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


	if(clicked && player->frameNumber == 0)
	{
		player->frameNumber = 1;
	}
	if(clicked && full_charge <= get_time())
	{
		player->frameNumber = 2;
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
		player->frameNumber = 0;
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

	if(get_time() > mutiplierDegradeTime)
	{
		multiplier--;
		if(multiplier < 1)
		{
			multiplier = 1;
		}
		multiplierDegradeRate += 1000;
		if(multiplierDegradeRate > multiplierDegradeMax)
		{
			multiplierDegradeRate = multiplierDegradeMax;
		}
		mutiplierDegradeTime = get_time() + multiplierDegradeRate;
	}

	if(lastPointReward + pointRewardRate < player->points)
	{
		lastPointReward += pointRewardRate;
		player_add_life();
	}

	
	// special player states handled here
	if(player->state == SHIELDED_STATE)
	{
		if(player->health <= 1)
		{
			player->state = NORMAL_STATE;
		}
		else
		{
			tempPos = vect2d_new(player->position.x - 10, player->position.y - 37);
			sprite_bloom_effect_draw(shield, 0, tempPos);
		}
	}
	else if(player->state == DEAD_STATE)
	{
		if(respawnMoment < get_time())
		{
			player->state = NORMAL_STATE;
			SDL_SetTextureAlphaMod(player->sprite->image, 255);
			player->health = 1;
		}
		else
		{
			SDL_SetTextureAlphaMod(player->sprite->image, 100 * (1 + sin(get_time() * 2 * 3.14 / 250)));
		}
	}


	//check if player has died, acts accordingly
	if(player->state == NORMAL_STATE && player->health <= 0)
	{
		audio_play_sound(player->entitySounds->death);
		if(player->inventory[LIVES] == 1)
		{
			//game over code here
			player->state = GAME_OVER_STATE;
			player_saved_load_off();
			camera_stop();
			return;
		}
		player->state = DEAD_STATE;


		multiplier = multiplierMin;
		multiplierDegradeRate = multiplierDegradeMin;
		mutiplierDegradeTime = get_time() + multiplierDegradeRate;
		
		player->inventory[LIVES]--;
		player->inventory[SPREADS] = 0;
		player->thinkRate = thinkRateMin;
		respawnMoment = get_time() + RESPAWN_RATE;
		return;
	}



	//movement input 
	keys = SDL_GetKeyboardState(NULL);
	if(keys[SDL_SCANCODE_Q])
	{
		player->position.x += 20000;
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
		if(!(player->position.y + 5 + player->sprite->frameSize.y > player->owner->position.y + HUD_HEIGHT)) // keeps pep from moving below camera, adds camera's y (0) to its height to get the position of the bottom of the camera
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

	if(player->inventory[LIVES] > 5)
	{
		player->inventory[LIVES] = 5;
	}


	//now that movement has been calculated, add it to the player's position, reset the player to be in the camera's bounds, if they are out
	vect2d_add(player->position, player->velocity, player->position);
	if(player->position.x < player->owner->position.x)
	{
		player->position.x = player->owner->position.x;
	}

	//check if player is touching anything
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

	if(!playerSavedLoad)
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
	cJSON_AddNumberToObject(buf, "points", player->points);
	cJSON_AddNumberToObject(buf, "health", player->health);
	memset(&temp[0], 0, sizeof(temp));
	sprintf(temp, "%f %f", player->maxVelocity.x, player->maxVelocity.y);
	cJSON_AddStringToObject(buf, "velMax", temp);
	cJSON_AddNumberToObject(buf, "lives", player->inventory[LIVES]);
	cJSON_AddNumberToObject(buf, "bombs", player->inventory[BOMBS]);
	cJSON_AddNumberToObject(buf, "spreads", player->inventory[SPREADS]);

	cJSON_AddNumberToObject(buf, "lastPointReward", lastPointReward);
	cJSON_AddNumberToObject(buf, "pointRewardRate", pointRewardRate);

	//if I want to let the multiplier to carry between levels I'd add it here to not be constants
	cJSON_AddNumberToObject(buf, "multiplier", 1);
	cJSON_AddNumberToObject(buf, "multiplierMax", 8);
	cJSON_AddNumberToObject(buf, "multiplierDegradeRate", 8000);
	cJSON_AddNumberToObject(buf, "multiplierDegradeMin", 1000);
	cJSON_AddNumberToObject(buf, "multiplierDegradeMax", 8000);

	cJSON_AddItemToObject(obj, "audioPak", buf = cJSON_CreateObject());
	cJSON_AddStringToObject(buf, "name", player->entitySounds->name);
	cJSON_AddStringToObject(buf, "firing1", player->entitySounds->firing1->filename);
	cJSON_AddStringToObject(buf, "firing2", player->entitySounds->firing2->filename);
	cJSON_AddStringToObject(buf, "death", player->entitySounds->death->filename);
	cJSON_AddStringToObject(buf, "moving", player->entitySounds->moving->filename);

	data = cJSON_Print(json);
	cJSON_Delete(json);

	player_save_config_file = fopen(PLAYER_SAVE_CONFIG, "w");

	if (player_save_config_file == NULL) 
	{
	  slog("Can't open output file %s!\n",
			  PLAYER_SAVE_CONFIG);
	  return;
	}

	fprintf(player_save_config_file, data);

	fclose(player_save_config_file);
}

void player_add_life()
{
	if(player->inventory[LIVES] < MAX_LIVES)
	{
		player->inventory[LIVES]++;
	}
}

void player_add_bomb()
{
	if(player->inventory[BOMBS] < MAX_BOMBS)
	{
		player->inventory[BOMBS]++;
	}
}

Entity *player_get()
{
	return player;
}

void player_saved_load_on()
{
	playerSavedLoad = 1;
}

void player_saved_load_off()
{
	playerSavedLoad = 0;
}

void player_reward_points(Uint32 points)
{
	player->points += points * multiplier;
	multiplier++;
	if(multiplier > 8)
	{
		multiplier = 8;
	}
	multiplierDegradeRate -= 1000;
	if(multiplierDegradeRate < multiplierDegradeMin)
	{
		multiplierDegradeRate = multiplierDegradeMin;
	}
	mutiplierDegradeTime = get_time() + multiplierDegradeRate;
}

Uint8 get_multiplier()
{
	return multiplier;
}

