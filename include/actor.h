#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "vector.h"
#include "entity.h"

/** @file	actor.h
 *	@brief	the class that handles drawing the sprites on the screen for the level editor, these are visual representations of what will be enemies and power ups when the game saves this level and loads it later
 */

/** 
 *  @struct Actor structure
 *	@brief a sprite that has a type and a position, used for the level editor so the player can place entities and then save their types and positions
 */
typedef struct Actor_s
{
	Uint8		alive;													/**< flag to know if the Actor is currently active */
	Uint8		type;													/**< Entity type check Entity.h for list of types */

	Sprite		*sprite;												/**< sprite component of the entity */

	Vect2d		position;												/**< x and y coordinates of the actor */
	
	void (*draw)	(Sprite *sprite, int frame, Vect2d drawPosition);	/**< draw function for the actor, most if not all actors will just be using sprite_draw */
	void (*update)	(struct Actor_s *self);								/**< update function for changing the actor's data and checking its status */
	void (*free)	(struct Actor_s **self);							/**< function to free the actor from the list */
}Actor;

/**
 * @brief clears out all actors from the actorList leaving the system intact
 */
void actor_empty_list();

/**
 * @brief frees an actor from the actor list, and its pointer
 * @param [in,out]	actor	the double pointer to the actor to be freed
 */
void actor_free(Actor **actor);

/**
 * @brief clears the actor list and then closes the system
 */
void actor_close_system();

/**
 * @brief initializes the system for actors by allocating space for the actorList
 * @param	maxActor the maximum number of actors the actorList will allow
 */
void actor_initialize_system(int maxActor);

/**
 * @brief creates a new actor of a given type at a given position
 * @param position	x and y coordinates of the actor
 * @param type		the type of the actor to load (corresponds to EntityTypes defined in entity.h)
 * @return a pointer to a loaded actor
 */
Actor *actor_new(Vect2d position, Uint8 type);

/**
 * @brief draws all the actors to the screen at their relative position to the camera
 */
void actor_draw_all();

/**
 * @brief updates all actors in the actorList
 */
void actor_update_all();

/**
 * @brief loads a sprite for a given type of actor and returns it
 * @param type	the type of actor whose sprite needs to be created and returned
 * @return a pointer to the sprite that has been loaded
 */
Sprite *actor_load_sprite(Uint8 type);

/**
 * @brief gets the number of actors in the actorList of a given type
 * @param type	the type of actors to count
 * @return the number of actors with the given type
 */
Uint8 actor_get_number_of_type(Uint8 type);

/**
 * @brief returns the position of the next actor in the actorList of the given type, will reset search after being given a new type
 * @param type	the type of actor to search for, given succesively will find the next actor of that type in the list and remmeber which position it was at on the last search
 * @return the posiiotn of the next actor in the list with the given type
 */
Vect2d actor_get_all_positions_of_type(Uint8 type);

#endif
