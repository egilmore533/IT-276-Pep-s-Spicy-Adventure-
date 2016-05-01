#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "entity.h"

/** @file	camera.h
 *	@brief	camera handling, camera in this case is a entity, camera is always moving right slowly, 
 *			camera is used in draw all to check if the entity is on screen (if not it sin't drawn)
 */


#define HUD_HEIGHT		618

/**
 * @brief	initializes the camera entity with cJSON and a predefined def file, needs to be initialized after
 * 			all the systems are started, but before the level is loaded.
 * @param	id		  	The camera entity's identifier.
 */
Entity *camera_load(int id);

/**
 * @brief	moves the camera, performs the intersect all to check what entities are inside the camera's bounds, 
 *          then camera_touch will draw them, be sure to remove camera_touch after runnign intersect all that way 
 *          nothing is drawn twice.
 * @param [in,out]	self	If non-null, the camera entity.
 */
void camera_update(Entity *self);

/**
 * @brief	getter for the camera entity so other files can use the camera.
 * @return	null if it fails, else an Entity*.
 */
Entity *camera_get();

/** 
 *  @brief	runs the entities free function if they are out of the camera's bounds
 *  @param ent	the entity in question
 */
void camera_free_entity_outside_bounds(Entity *ent);

/** @brief	stops the camera from moving */
void camera_stop();

/** @brief sets the camera to move */
void camera_move();

/**
 * @brief frees the camera from memory 
 * @param [in,out] free the camera that needs to be freed
 */
void camera_free(Entity *free);

/**
 * @brief returns the status of the camera (whether or not its moving)
 * @return the status of the camera's movement
 */
Uint8 camera_get_move();

/**
 * @brief gets the gametime to use for sin waves and all functions that concerns time
 * @return	the gametime
 */
Uint32 get_time();

#endif
