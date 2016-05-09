#ifndef __FILES_H__
#define __FILES_H__

/** @file	files.h
 *	@brief	defines all files for easy filepaths used in configuration, level loading, logging, etc
 */

//////////////////////////////LEVEL CONFIG FILES///////////////////////////////////////
#define LEVEL_NUM	3					/**< the number of levels */

#define LEVEL_1							"def/levels/tutorial_level.txt"	
#define LEVEL_2							"def/levels/melt_level.txt"
#define LEVEL_3							"def/levels/clarence_level.txt"

#define CHALLENGE_LEVEL					"def/levels/challenge_level.txt"

#define ENDLESS_LEVEL					"def/levels/level_config_template.txt"

//////////////////////////////HIGHSCORE CONFIG FILES///////////////////////////////////
#define ARCADE_HIGHSCORES				"def/highscores/arcade_highscores.txt"
#define CHALLENGE_HIGHSCORES			"def/highscores/challenge_highscores.txt"

////////////////////////////////SLOG FILES////////////////////////////////////////////
#define LOG_FILE						"text/log.txt"

/////////////////////////////////ENTITY CONFIG FILES//////////////////////////////////
#define ENEMY_CONFIG					"def/entities/enemy_config.txt"
#define PLAYER_CONFIG					"def/entities/player_config.txt"
#define PLAYER_SAVE_CONFIG				"def/entities/player_save_config.txt"
#define POWER_UP_CONFIG					"def/entities/power_up_config.txt"
#define CAMERA_CONFIG					"def/entities/camera_config.txt"
#define WEAPON_CONFIG					"def/entities/weapon_config.txt"

////////////////////////////////PARTICLE CONFIG FILES////////////////////////////////
#define DUST_PAK_CONFIG					"def/particles/dust_config.txt"

//////////////////////////////BACKGROUND CONFIG FILES////////////////////////////////
#define SUNNY_PEPS_CONFIG				"def/backgrounds/sunny_peps_config.txt"

/////////////////////////////ACTOR CONFIG FILES/////////////////////////////////////
#define ACTOR_CONFIG					"def/entities/actor_config.txt"

////////////////////////////FONT FILES///////////////////////////////////////////////////////
#define PEP_FONT						"fonts/HussarPrintASpicyAdventure.ttf"

////////////////////////////SOUND FILES//////////////////////////////////////////////////////////
#define BUTTON_HOVER_NOISE				"sound/button_hover_noise.ogg"

/**
 *	@brief	given a level number select and return the appropriate level path
 *	@param	num		the level number
 *	@return char pointer for the level's filepath
 */
char *files_get_level(int num);


#endif