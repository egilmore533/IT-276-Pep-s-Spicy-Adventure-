#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "audio.h"
#include "simple_logger.h"
#include <SDL.h>

static int numSounds = 0;
static int maxSounds = 0;
static int numMusics = 0;
static int maxMusics = 0;
static int numPaks = 0;
static int maxPaks = 0;
static Sound *soundList = NULL;
static Music *musicList = NULL;
static SoundPak *pakList = NULL;

void audio_music_free(Music **music)
{
	Music *target;
	if(!music)
	{
		return;
	}
	else if(!*music) 
	{
		return;
	}
	target = *music;
	target->used--;

	if(target->refCount == 0)
	{
		strcpy(target->filename,"\0");
		
		if(target->music != NULL)
		{
			Mix_FreeMusic(target->music); 
			memset(target,0,sizeof(Music));
		}
		numMusics--;
	}
	*music = NULL;
}

void audio_sound_free(Sound **sound)
{
	Sound *target;
	if(!sound)
	{
		return;
	}
	else if(!*sound) 
	{
		return;
	}
	target = *sound;
	target->used--;

	if(target->refCount == 0)
	{
		strcpy(target->filename,"\0");
		
		if(target->sound != NULL)
		{
			Mix_FreeChunk(target->sound); 
			memset(target,0,sizeof(Sound));
		}
		numSounds--;
	}
	*sound = NULL;
}

void audio_close_music()
{
	int i;
	Music *music;
	if(!musicList)
	{
		slog("musicList not initialized");
		return;
	}
	for(i = 0; i < maxMusics; i++)
	{
		music = &musicList[i];
		audio_music_free(&music);
	}
	maxMusics = 0;
	free(musicList); 
	musicList = NULL;
}

void audio_close_sound()
{
	int i;
	Sound *sound;
	if(!soundList)
	{
		slog("soundList not initialized");
		return;
	}
	for(i = 0; i < maxSounds; i++)
	{
		sound = &soundList[i];
		audio_sound_free(&sound);
	}
	maxSounds = 0;
	free(soundList); 
	soundList = NULL;
}

void audio_close_lists()
{
	audio_close_music();
	audio_close_sound();
	audio_close_paks();
	Mix_CloseAudio();
}

void audio_initialize(int soundMax, int musicMax, int pakMax)
{
	int flags=MIX_INIT_OGG;
	if(soundMax <= 0 || musicMax <= 0)
	{
		slog("cannot initialize for 0 sounds or 0 musics");
		return;
	}
	if(Mix_Init(flags) != flags)
	{
		slog("Not initialized for ogg");
		return;
	}
	if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
	{
		slog("failed to open audio");
		return;
	}
	atexit(audio_close_lists);

	if(Mix_AllocateChannels(5) != 5)
	{
		slog("unable to allocate enough channels");
		return;
	}

	musicList = (Music *) malloc (sizeof (Music) * musicMax);
	soundList = (Sound *) malloc (sizeof (Sound) * soundMax);
	pakList = (SoundPak *) malloc (sizeof (SoundPak) * pakMax);

	memset(musicList, 0, sizeof (Music) * musicMax);
	memset(soundList, 0, sizeof (Sound) * soundMax);
	memset(pakList, 0, sizeof (SoundPak) * pakMax);

	maxMusics = musicMax;
	maxSounds = soundMax;
	maxPaks = pakMax;
}

Music *audio_load_music(char *filename, int loop)
{
	int i;
	Music *music = NULL;

	if(!musicList)
	{
		slog("soundList uninitialized");
		return NULL;
	}
	for(i = 0; i < maxMusics; i++)
	{
		if(musicList[i].refCount == 0)
		{
			if(music == NULL)
				music = &musicList[i];
			continue;
		}
		if(strncmp(filename, musicList[i].filename, 80) ==0)
		{
			musicList[i].refCount++;
			return &musicList[i];
		}
	}
	if(numMusics  + 1 > maxMusics)
	{
		slog("Maximum musics reached");
		exit(1);
	}

	numMusics++;

	strcpy(music->filename, filename);
	music->music = Mix_LoadMUS(filename);
	music->refCount++;
	music->loop = loop;
	return music;
}

Sound *audio_load_sound(char *filename, int loop, int channel)
{
	int i;
	Sound *sound = NULL;

	if(!soundList)
	{
		slog("soundList uninitialized");
		return NULL;
	}
	for(i = 0; i < maxSounds; i++)
	{
		if(soundList[i].refCount == 0)
		{
			if(sound == NULL)
				sound = &soundList[i];
			continue;
		}
		if(strncmp(filename, soundList[i].filename, 80) ==0)
		{
			soundList[i].refCount++;
			return &soundList[i];
		}
	}
	if(numSounds  + 1 > maxSounds)
	{
		slog("Maximum sounds reached");
		exit(1);
	}

	numSounds++;

	strcpy(sound->filename, filename);
	sound->channel = channel;
	sound->loop = loop;
	sound->refCount++;
	sound->sound = Mix_LoadWAV(filename);
	return sound;
}

void audio_play_music(Music *music)
{
	Mix_PlayMusic(music->music, music->loop);
}

void audio_play_sound(Sound *sound)
{
	Mix_PlayChannel(sound->channel, sound->sound, sound->loop);
}

void audio_close_paks()
{
	int i;
	SoundPak *soundPak;
	if(!pakList)
	{
		slog("pak List not initialized");
		return;
	}
	for(i = 0; i < maxPaks; i++)
	{
		soundPak = &pakList[i];
		audio_pak_free(&soundPak);
	}
	maxPaks = 0;
	free(pakList); 
	pakList = NULL;
}

SoundPak *audio_load_pak(int channel, char *name, char *fire1_file, char *fire2_file, char *death_file, char *moving_fire)
{
	int i;
	SoundPak *pak = NULL;

	if(!pakList)
	{
		slog("soundList uninitialized");
		return NULL;
	}
	for(i = 0; i < maxPaks; i++)
	{
		if(pakList[i].refCount == 0)
		{
			if(pak == NULL)
				pak = &pakList[i];
			continue;
		}
		if(strncmp(name, pakList[i].name, 80) ==0)
		{
			pakList[i].refCount++;
			return &pakList[i];
		}
	}
	if(numPaks  + 1 > maxPaks)
	{
		slog("Maximum paks reached");
		exit(1);
	}

	numSounds++;

	strcpy(pak->name, name);
	pak->firing1 = audio_load_sound(fire1_file, 0, FX_Bullets);
	pak->firing2 = audio_load_sound(fire2_file, 0, FX_Bullets);
	pak->death = audio_load_sound(death_file, 0, channel);
	pak->moving = audio_load_sound(moving_fire, 0, channel);
	pak->loaded = 1;
	pak->refCount++;
	return pak;
}

void audio_pak_free(SoundPak **pak)
{
	SoundPak *target;
	if(!pak)
	{
		return;
	}
	else if(!*pak) 
	{
		return;
	}
	target = *pak;
	target->loaded--;

	if(target->refCount == 0)
	{
		strcpy(target->name,"\0");
		
		if(target->moving != NULL)
		{
			audio_sound_free(&target->moving); 
		}
		if(target->firing1 != NULL)
		{
			audio_sound_free(&target->firing1);
		}
		if(target->firing2 != NULL)
		{
			audio_sound_free(&target->firing2);
		}
		if(target->death != NULL)
		{
			audio_sound_free(&target->death);
		}
		memset(target,0,sizeof(SoundPak));
		numPaks--;
	}
	*pak = NULL;
}
