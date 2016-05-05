#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "vector.h"
#include "entity.h"

/** @file	actor.h
 *	@brief	the class that handles drawing the sprites on the screen for the level editor, these are visual representations of what will be enemies and power ups when the game saves this level and loads it later
 */

/** @struct Actor structure
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

void actor_empty_list();

void actor_free(Actor **actor);

void actor_close_system();

void actor_initialize_system(int maxActor);

Actor *actor_new(Vect2d position, Uint8 type);

void actor_draw_all();

void actor_update_all();

Sprite *actor_load_sprite(Uint8 type);


#endif
