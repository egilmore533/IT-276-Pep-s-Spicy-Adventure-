#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/** @file	player.h
 *	@brief	Player handling, also handles deaths and respawns
 */

#define CHARGE_RATE			1000			/**< the rate at which the player charges their attack */

#define RESPAWN_RATE		2000			/**< how long the player will remain in a deceased state */

/**
 * @brief loads a player using config data found in the player_config.txt file and returning the player to have its position and id set
 * @return a pointer to the player static member
 */
Entity *player_load();

/**
 * @brief	fires by clicking, holding click for a certain amount of time will result in a charged attack, press space to use a
 *				bomb which will clear the screen of enemies
 * @param [in,out]	player	If non-null, the player.
 */
void player_think(Entity *player);

/**
 * @brief	sets the player's velocity according to what wasd keys are pressed, then moves the player accordingly, but keeps the player inside the camera's bounds.
 *			  also activates/deactivates the player's shield sprite, and handles the player being damaged as well as dying
 * @param [in,out]	player	the player entity.
 */
void player_update(Entity *player);

/** @brief adds an extra life for the player, caps add the MAX_LIVES */
void player_add_life();

/** @brief adds an extra bomb for the player, caps add the MAX_BOMBS */
void player_add_bomb();

/** 
 *	@brief gets the player entity so it can be used elsewhere in the code like the game loop
 *	@return	a pointer to the player
 */
Entity *player_get();

/**
 * @brief saves the player's info and states to a player_saved_config file, suing cJSON
 */
void player_save_info();

/**
 * @brief sets the player_load to load from the saved config file instead of the normal config file
 */
void player_saved_load_on();

/**
 * @brief sets the player_load to load from the normal config, instead of  the saved config
 */
void player_saved_load_off();

/**
 * @brief rewars the player enityt with the given number of points times the multiplier
 * @param points	how many points the enmey who dies rewars the player by normally
 */
void player_reward_points(Uint32 points);

/**
 * @brief gets the multiplier for the hud, to draw to the screen the multiplier grade
 * @return the multiplier, max of 8, min of 1 (F to SSS respectively)
 */
Uint8 get_multiplier();

#endif