#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

/** @file	enemy.h
 *	@brief	Each enemies functions for thinking, updates, touching, and a load function which will set the function pointers 
 *          accordingly for the enemy
 */

/**
 * @brief loads a given entity from a config file (sprite, info, and audioPak), and gives the entity the correct function pointer, all based on its type (Defined in EntityTypes)
 * @param type	the enemy type to be loaded, what the config file will be searched and what enemy load function will be called
 * @param [in, out] enemy	the entity that will be loaded to the given entity type
 * @return the entity with the correct function pointers
 */
Entity *enemy_load(int type, Entity *enemy);

/////////////////////////////CELERY STALKER CODE////////////////////////////////////

/**
 * @brief loads a celery_stalker entity's function pointers, sets its owner and state
 * @param [in] celery_stalker	the celery_stalker entity that needs to have its function pointers initialized
 * @return	the celery_stalker entity that now has function pointers set
 */
Entity *celery_stalker_load(Entity *celery_stalker);

/** 
 *  @brief waits for the specified number of ticks, then targets the player by setting the direction towards the player
 *  @param celery_stalker	the entity that is assumed to be a celery_stalker
 */
void celery_stalker_think_start(Entity *celery_stalker);

/** 
 *  @brief check if celery_stalker is touching the camera and delete it otherwise
 *  @param celery_stalker	the entity that is assumed to be a celery_stalker
 */
void celery_stalker_think_moving(Entity *celery_stalker);

/** 
 *  @brief adds the velocity result to position, kills celery_stalker if out of health, also intersects all with celery_stalker
 *  @param celery_stalker	the entity that is assumed to be a celery_stalker
 */
void celery_stalker_update(Entity *celery_stalker);

/** 
 *  @brief checks if the celery_stalker touches the camera, the player, or the player's attacks performs the appropriate actions for each,
 *			if the celery_stalker hasn't touched the camera yet it needs to begin thinking, player needs to get hurt, or the celery_stalker 
 *          needs to get hurt
 *  @param celery_stalker	the entity that is assumed to be a celery_stalker
 *  @param other 			the entity that collided with the celery_stalker
 */
void celerly_stalker_touch(Entity *celery_stalker, Entity *other);

//////////////////////////////////CLARENCE CODE////////////////////////////////////////

/**
 * @brief loads a clarence entity's function pointers, sets its owner and state
 * @param [in] clarence	the clarence entity that needs to have its function pointers initialized
 * @return	the clarence entity that now has function pointers set
 */
Entity *clarence_load(Entity *clarence);

/** 
 *  @brief  checks if clarence's target is directly below him, if so then sets the direction to be straight down and change to the other think, 
 *          also allow clarence to die
 *  @param	clarence	the clarence
 */
void clarence_think_start(Entity *clarence);

/** 
 *  @brief  check if clarence has left the camera's bounds, if so kill clarence
 *  @param	clarence	the clarence
 */
void clarence_think_moving(Entity *clarence);

/** 
 *  @brief  adds clarence's velocity to his position, kills clarence if he is out of health
 *  @param	clarence	the clarence 
 */
void clarence_update(Entity *clarence);

/** 
 *  @brief  checks if the thing clarence is touching is the player or the player's weapon and acts accordingly
 *  @param	clarence	the clarence
 */
void clarence_touch(Entity *clarence, Entity *other);

////////////////////////////////MELT CODE////////////////////////////////////

/**
 * @brief loads a melt entity's function pointers, sets its owner and state
 * @param [in] melt	the melt entity that needs to have its function pointers initialized
 * @return	the melt entity that now has function pointers set
 */
Entity *melt_load(Entity *melt);

/** 
 *  @brief  fires projectiles according to the think rate and sets the melt's direction and velocity to be towards the player
 *  @param	melt	the melt
 */
void melt_think(Entity *melt);

/**
 *  @brief	 melt will be stickied until the nextThink, then will reset the think function and its think rate back to normal
 *  @param	[in,out] melt	the  melt
 */
void melt_stickied_think(Entity *melt);

/** 
 *  @brief	adds melt's velocity to its position, kills melt if not touching the camera
 *  @param	melt	the melt
 */
void melt_update(Entity *melt);

/** 
 *  @brief  checks if the melt is touching the player, or the camera and acts according
 *  @param	melt	the melt
 *  @param	other	the entity melt touched
 */
void melt_touch(Entity *melt, Entity *other);

/////////////////////////////MILK TANK CODE///////////////////////////////////////

/**
 * @brief loads a milk_tank entity's function pointers, sets its owner and state
 * @param [in] milk_tank	the milk_tank entity that needs to have its function pointers initialized
 * @return	the milk_tank entity that now has function pointers set
 */
Entity *milk_tank_load(Entity *milk_tank);

/**
 * @brief	calculates the magnitude of the separation between milk_tank and player, then normalizes that magnitude to get the direction 
 *			milk_tank should move to go straight towards the player, then multiplies that direction by the defined velocity
 * @param [in,out]	milk_tank the milk tank that will be thinking.
 */
void milk_tank_think(Entity *milk_tank);

/**
 *  @brief	 milk_tank will be stickied until the nextThink, then will reset the think function and its think rate back to normal
 *  @param	 milk_tank	the  milk_tank
 */
void  milk_tank_stickied_think(Entity * milk_tank);

/**
 * @brief	adds milk_tank's velocity to its position, and kills milk_tank if has no more health
 * @param [in,out]	milk_tank	If non-null, the milk tank entity.
 */
void milk_tank_update(Entity *milk_tank);

/**
 * @brief	frees the milk tank's function pointers and uses entity_free to handle the rest of the freeing
 * @param [in,out]	milk_tank	If non-null, the milk tank.
 */
void milk_tank_free(Entity *milk_tank);

/**
 * @brief	what happens when the milk_tank touches the player or camera
 * @param [in,out]	milk_tank	the milk tank.
 * @param [in,out]	other	 	the other.
 */
void milk_tank_touch(Entity *milk_tank, Entity *other);

//////////////////////////////PROFESSOR SLICE CODE//////////////////////////////////

/**
 * @brief loads a professor_slice entity's function pointers, sets its owner and state
 * @param [in] professor_slice	the professor_slice entity that needs to have its function pointers initialized
 * @return	the professor_slice entity that now has function pointers set
 */
Entity *professor_slice_load(Entity *professor_slice);

/** 
 *  @brief  fires projectiles according at the think rate and sets the professor_slice's direction and velocity to be away from the player, 
 *          kills professor_slice if not touching the camera
 *  @param	professor_slice	the professor_slice
 */
void professor_slice_think(Entity *professor_slice);

/**
 *  @brief	professor_slice will be stickied until the nextThink, then will reset the think function back to normal
 *  @param	professor_slice	the professor_slice
 */
void professor_slice_stickied_think(Entity *professor_slice);

/** 
 *  @brief	adds professor_slice's velocity to its position 
 *  @param	professor_slice the professor_slice
 */
void professor_slice_update(Entity *professor_slice);

/** 
 *  @brief  checks if the professor_slice is touching the camera (and doesn't have a think function set), or the player and acts according
 *  @param	professor_slice	the professor_slice
 *  @param	other		    the entity professor_slice touched
 */
void professor_slice_touch(Entity *professor_slice, Entity *other);

#endif
