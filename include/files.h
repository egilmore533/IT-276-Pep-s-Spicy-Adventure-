#ifndef __FILES_H__
#define __FILES_H__

/** @file	files.h
 *	@brief	def file for easy filepaths used in configuration and level loading
 */

//levels
#define LEVEL_NUM	3

#define LEVEL_1							"def/levels/tutorial_level.txt"
#define LEVEL_2							"def/levels/melt_level.txt"
#define LEVEL_3							"def/levels/clarence_level.txt"

#define CHALLENGE_LEVEL					"def/levels/challenge_level.txt"

/**
 *	@brief	given a level number select and return the appropriate level path
 *	@param	num		the level number
 *	@return char pointer for the level's filepath
 */
char *files_get_level(int num);

#define LOG_FILE						"text/log.txt"

//entities
#define ENEMY_CONFIG					"def/entities/enemy_config.txt"
#define PLAYER_CONFIG					"def/entities/player_config.txt"
#define PLAYER_SAVE_CONFIG				"def/entities/player_save_config.txt"
#define POWER_UP_CONFIG					"def/entities/power_up_config.txt"
#define CAMERA_CONFIG					"def/entities/camera_config.txt"
#define WEAPON_CONFIG					"def/entities/weapon_config.txt"

//particles
#define DUST_PAK_CONFIG					"def/particles/dust_config.txt"

//backgrounds
#define SUNNY_PEPS_CONFIG				"def/backgrounds/sunny_peps_config.txt"

#define ACTOR_CONFIG					"def/entities/actor_config.txt"

#endif