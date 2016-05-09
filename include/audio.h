#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SDL_mixer.h>

/** @file	audio.h
 *	@brief	Audio system with Music and Sound resource management and audio playback
 */

/**
 * @enum FX_Groups
 * @brief the different channels that the audio sounds will be played on
 */
typedef enum	
{
	FX_NULL = 0, 
	FX_BULLETS, 
	FX_IMPACTS, 
	FX_ENEMY, 
	FX_PLAYER
}FX_Groups;

/**
 * @struct Sound
 * @brief a Mix_Chunk with a filename, channel, and loop number
 */
typedef struct Sound_s
{
	Mix_Chunk *sound;		/**< The actual sound that will be played */
	char filename[80];		/**< the filename where the audio sound was loaded from, used to id this sound so it isn't loaded twice */
	int used;				/**< the flag to let the resource manager know if this specific sound was loaded */
	int refCount;			/**< the number of times this piece of sound is currently being used */
	float volume;			/**< the volume this sound will be played at */
	int loop;				/**< the numebr of times to loop this sound */
	int channel;			/**< the channel this sound will be played on */
}Sound;

/**
 * @struct Music
 * @brief a Mix_Music with a filename, channel, and loop number
 */
typedef struct Music_s
{
	Mix_Music *music;		/**< the actual music that will be played */
	char filename[80];		/**< the filename where the music was loaded from, used to id this music so it isn't loaded twice */
	int used;				/**< the flag to let the resource manager know if this specific music has been loaded */
	int refCount;			/**< the number of times this piece of music is currently being used */
	float volume;			/**< the volume this music will be played at */
	int loop;				/**< the number of times to loop this music */
}Music;

/**
 * @struct SoundPak
 * @brief a pak of sounds for the entities to use as needed, not yet implemeted in any fashion
 */
typedef struct SoundPak_S
{
	char name[80];			/**< name of this SoundPak, used to id this SoundPak */
	int loaded;				/**< flag to let the resource manager know if this specific soundPak has been loaded */
	int refCount;			/**< the number of times this SoundPak has been used */
	Sound *moving;			/**< sound to be played when the entity moves */
	Sound *firing1;			/**< firing sound to be played when the entity is firing */
	Sound *firing2;			/**< alternate firing sound to be played when the entity is firing */
	Sound *death;			/**< sound to be played when the entity dies */
}SoundPak;

/**
 * @brief initializes the audio system with given parameters
 * @param soundMax  maximum amount of sounds to be loaded
 * @param musicMax  maximum amount of music to be loaded
 * @param pakMax	maximum amount of soundPaks that can be loaded
 */
void audio_initialize(int soundMax, int musicMax, int pakMax);

/** @brief closes both the musicList and the audioList */
void audio_close_lists();

/** @brief closes the musicList, called by audio_close_lists */
void audio_close_music();

/** @brief closes the soundList, called by audio_close_lists */
void audio_close_sound();

/** 
 * @brief frees a music from the musicList 
 * @param [in,out] music  a double pointer to the music to be freed
 */
void audio_music_free(Music **music);

/**
 * @brief frees a sound from the soundList
 * @param [in,out] sound  a double pointer to the sound to be freed
 */
void audio_sound_free(Sound **sound);

/**
 * @brief loads a sound file into the soundList
 * @param filename - the name of the file to be loaded
 * @param loop	   - the number of times to loop the audio (-1 forever)
 * @param channel  - the channel number for this audio file
 * @return the loaded Sound data
 */
Sound *audio_load_sound(char *filename, int loop, int channel);

/**
 * @brief loads a music file into the musicList
 * @param filename - the name of the file to be loaded
 * @param loop	   - the number of times to loop the audio (-1 forever)
 * @return the loaded Music data
 */
Music *audio_load_music(char *filename, int loop);

/** 
 * @brief plays the given music, loops the number of times it was defined to
 * @param [in] music  the music to be played
 */
void audio_play_music(Music *music);

/**
 * @brief plays the given sound, loops the number of times it was defined to
 * @param [in] sound  the sound to be played
 */
void audio_play_sound(Sound *sound);

/** @brief closes the pakList, called by audio_close_lists */
void audio_close_paks();

/**
 * @brief loads an SoundPak into the pakList with the given information, checks to see if there are any paks in the pakList that already match the one being loaded and references that instead
 * @param channel			the channel that the SoundPak's sounds will belong to
 * @param [in]	name		the name of SoundPak, used to id others
 * @param [in]	fire1_file	the path for the fire1 sound
 * @param [in]	fire2_file	the path for the fire2 sound
 * @param [in]	death_file	the path for the death sound
 * @param [in]	moving_file	the path for the moving sound
 * @return the SoundPak
 */
SoundPak *audio_load_pak(int channel, char *name, char *fire1_file, char *fire2_file, char *death_file, char *moving_fire);

/**
 * @brief frees a reference of the given SoundPak, if the only reference in pakList it will free the SoundPak from the pakList as well
 * @param [in,out] pak	double pointer to the pak to be freed
 */
void audio_pak_free(SoundPak **pak);

/**
 * @brief empties all the audio lists leaving the system in tact, calls audio_empty_music_list, audio_empty_sound_list, and audio_empty_pak_list
 */
void audio_empty_list();

/**
 * @brief empties the musicList leaving the system in tact
 */
void audio_empty_music_list();

/**
 * @brief empties the soundList leaving the system in tact
 */
void audio_empty_sound_list();

/**
 * @brief empties the pakList leaving the system in tact
 */
void audio_empty_pak_list();


#endif