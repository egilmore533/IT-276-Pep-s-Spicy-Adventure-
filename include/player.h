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
 * @param position	the position the player will be spawned in
 * @param id		the player's id
 */
Entity *player_load(Entity *player, Vect2d position, int id);

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

/** 
 *  @brief	frees the player's function pointers and then uses entity_free to handle the remaining freeing
 *  @param [in,out]	player	the player entity. 
 */
void player_free(Entity *player);

/**
 * @brief player's death function, used to respawn the player at the appropriate position
 * @param id		the player's id so it can be refound by the other entities that need to find it
 * @param pos		the pos of the player at the time of its death, only the y is used because if using the x the player could be spawned outside of the level bounds
 */
void player_death(int id, Vect2d pos);

/** @brief adds an extra life for the player */
void player_add_life();

/** @brief gets the player entity so it can be used elsewhere in the code like the game loop, defeats the purpose of having a get entity by id, but getting entity by the id could be useful to have elsewhere */
Entity *player_get();

#endif