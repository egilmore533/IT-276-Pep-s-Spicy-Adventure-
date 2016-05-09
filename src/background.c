#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"
#include "simple_logger.h"

#include "background.h"
#include "camera.h"

/* background resource manager */
static Background	*backgroundList = NULL;
static int			backgroundNum;
static int			backgroundMax = 0;

void background_free(Background **background)
{
	Background *target;
	if(!background)
	{
		return;
	}
	else if(!*background)
	{
		return;
	}
	target = *background;
	target->alive--;

	if(target->sprite != NULL)
	{
		sprite_free(&target->sprite);
	}
	backgroundNum--;
	*background = NULL;
}

void background_pak_free(BackgroundPak *pak)
{
	if(pak->front && pak->front->free)
		pak->front->free(&pak->front);
	if(pak->middle && pak->middle->free)
		pak->middle->free(&pak->middle);
	if(pak->back && pak->back->free)
		pak->back->free(&pak->back);
	if(pak->flair && pak->flair->free)
		pak->flair->free(&pak->flair);
}

void background_close_system()
{
	int i;
	Background *background;
	if(!backgroundList)
	{
		slog("backgroundList not initialized");
		return;
	}
	for(i = 0; i < backgroundMax; i++)
	{
		background = &backgroundList[i];
		background_free(&background);
	}
	backgroundNum = 0;
	free(backgroundList);
	backgroundList = NULL;
}

void background_initialize_system(int maxBackground)
{
	int i;
	if(maxBackground == 0)
	{
		slog("Max Background == 0");
		return;
	}
	backgroundList = (Background *) malloc (sizeof (Background) * maxBackground);
	if(!backgroundList)
	{
		slog("failed to initialize backgroundList");
		return;
	}
	memset(backgroundList, 0, sizeof (Background) * maxBackground);
	backgroundNum = 0;
	for(i = 0; i < maxBackground; i++)
	{
		backgroundList[i].sprite = NULL;
	}
	backgroundMax = maxBackground;
	atexit(background_close_system);
}

Background *background_new(float moveFactor, Uint32 nextThink, Uint32 thinkRate)
{
	int i;
	Entity *cam = camera_get();
	if(backgroundNum + 1 > backgroundMax)
	{
		slog("Maximum background reached");
		exit(1);
	}
	for(i = 0; i < backgroundMax; i++)
	{
		if(backgroundList[i].alive)
		{
			continue;
		}
		memset(&backgroundList[i],0,sizeof(Background));
		backgroundList[i].alive = 1;
		backgroundList[i].velocity = vect2d_new(moveFactor * cam->velocity.x, moveFactor * cam->velocity.y);
		vect2d_negate(backgroundList[i].velocity, backgroundList[i].velocity);
		backgroundList[i].moving = camera_get_move();
		backgroundList[i].nextThink = nextThink;
		backgroundList[i].thinkRate = thinkRate;

		//standard functions for every background
		backgroundList[i].update = &background_update;
		backgroundList[i].free = &background_free;
		backgroundList[i].draw = &sprite_draw;
		backgroundNum++;
		return &backgroundList[i];
	}
	return NULL;
}

BackgroundPak *background_pak_new(char *file)
{
	BackgroundPak *pak = NULL;
	int i;

	//config file stuff
	cJSON *json, *root, 
		*obj,  *buf;
	FILE *background_pak_config_file;
	long length;
	char *data;
	char *string;

	//background info
	float moveFactor;
	Uint32 nextThink;
	Uint32 thinkRate;

	//background sprite info
	char *filepath;
	Vect2d frameSize;
	int frame;
	int fpl;
	int frames;

	background_pak_config_file = fopen(file,"r");
	if(!background_pak_config_file)
	{
		slog("No file found: %s", file);
		return NULL;
	}
	//get the length of the file
	fseek(background_pak_config_file, 0, SEEK_END);
	length = ftell(background_pak_config_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(background_pak_config_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, background_pak_config_file);
	fclose(background_pak_config_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"background_config");

	if(!root)
	{
		slog("error parseing the file, file not a background_config");
		return NULL;
	}

	pak = (BackgroundPak *) malloc (sizeof (BackgroundPak));
	memset(pak, 0, sizeof (BackgroundPak));

	for(i = 0; i < 4; i++)
	{
		//initializing the obj
		switch(i)
		{
			case 0:
				obj = cJSON_GetObjectItem(root, "back");
				if(!obj)
				{
					slog("no back for the pak");
					continue;
				}
				break;
			case 1:
				obj = cJSON_GetObjectItem(root, "middle");
				if(!obj)
				{
					slog("no middle for the pak");
					continue;
				}
				break;
			case 2:
				obj = cJSON_GetObjectItem(root, "front");
				if(!obj)
				{
					slog("no front for the pak");
					continue;
				}
				break;
			case 3:
				obj = cJSON_GetObjectItem(root, "flair");
				if(!obj)
				{
					slog("no flair for the pak");
					continue;
				}
				break;
		}

		pak->free = &background_pak_free;

		buf = cJSON_GetObjectItem(obj, "info");
		moveFactor = cJSON_GetObjectItem(buf, "moveFactor")->valuedouble;
		thinkRate = cJSON_GetObjectItem(buf, "thinkRate")->valueint;
		nextThink = cJSON_GetObjectItem(buf, "nextThink")->valueint;

		buf = cJSON_GetObjectItem(obj, "sprite");
		filepath = cJSON_GetObjectItem(buf, "file")->valuestring;
		sscanf(cJSON_GetObjectItem(buf, "bounds")->valuestring, "%f %f", &frameSize.x, &frameSize.y);
		frame = cJSON_GetObjectItem(buf, "frame")->valueint;
		fpl = cJSON_GetObjectItem(buf, "fpl")->valueint;
		frames = cJSON_GetObjectItem(buf, "frames")->valueint;

		//loading for the specific obj
		switch(i)
		{
			case 0:
				pak->back = background_new(moveFactor, nextThink, thinkRate);
				pak->back->sprite = sprite_load(filepath, frameSize, fpl, frames);
				break;
			case 1:
				pak->middle = background_new(moveFactor, nextThink, thinkRate);
				pak->middle->sprite = sprite_load(filepath, frameSize, fpl, frames);
				break;
			case 2:
				pak->front = background_new(moveFactor, nextThink, thinkRate);
				pak->front->sprite = sprite_load(filepath, frameSize, fpl, frames);
				break;
			case 3:
				pak->flair = background_new(moveFactor, nextThink, thinkRate);
				pak->flair->sprite = sprite_load(filepath, frameSize, fpl, frames);
				break;
		}
	}
	return pak;

}

void background_draw_all()
{
	int i;
	for(i = 0; i < backgroundMax; i++)
	{
		if(!backgroundList[i].alive)
		{
			continue;
		}
		if(!backgroundList[i].draw)
		{
			continue;
		}
		backgroundList[i].draw(backgroundList[i].sprite, backgroundList[i].frameNumber, backgroundList[i].mainPosition);
		backgroundList[i].draw(backgroundList[i].sprite, backgroundList[i].frameNumber, backgroundList[i].chaserPosition);
	}
}

void background_update_all()
{
	int i;
	for(i = 0; i < backgroundMax; i++)
	{
		if(!backgroundList[i].alive)
		{
			continue;
		}
		if(!backgroundList[i].update)
		{
			continue;
		}
		backgroundList[i].update(&backgroundList[i]);
	}
}

void background_update(Background *background)
{
	vect2d_add(background->mainPosition, background->velocity, background->mainPosition);
	vect2d_add(background->chaserPosition, background->velocity, background->chaserPosition);
	
	if(background->chaserPosition.x <= camera_get()->position.x)
	{
		background->mainPosition = background->chaserPosition;
		background->chaserPosition = vect2d_new(background->mainPosition.x + background->sprite->frameSize.x, 0);

	}
	if(get_time() > background->nextThink)
	{
		if(background->frameNumber + 1 < background->sprite->frames)
		{
			background->frameNumber++;
		}
		else
		{
			background->frameNumber = 0;
		}
		background->nextThink = get_time() + background->thinkRate;
	}
}

void background_empty_list()
{
	int i;
	Background *background = NULL;
	if(!backgroundList)
	{
		slog("backgroundList not initialized");
		return;
	}
	for(i = 0; i < backgroundMax; ++i)
	{
		background = &backgroundList[i];
		background_free(&background);
	}
	memset(backgroundList, 0, sizeof(Background) * backgroundMax);
	for(i = 0; i < backgroundMax; ++i)
	{
		backgroundList[i].sprite = NULL;
	}
	backgroundNum = 0;
}
