#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/** @file	player.h
 *	@brief	Player handling, also handles deaths and respawns
 */

#define CHARGE_RATE			1000
#define RESPAWN_RATE		1000

#define NORMAL_STATE		0
#define SHIELDED_STATE		1

/**
 * @brief loads a player using config data found in the player_config.txt file setting it to the given position and id
 * @param [in] p	the player entity that is having its function pointers initialized, will also set the player static member to be p for later use
 * @return a pointer to the player static member
 */
Entity *player_load(Entity *p);

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

/** @brief adds an extra life for the player */
void player_add_life();

/** @brief gets the player entity so it can be used elsewhere in the code like the game loop
 *	@return	a pointer to the player
 */
Entity *player_get();

#endif