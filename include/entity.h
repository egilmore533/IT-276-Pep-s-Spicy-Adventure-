#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "sprite.h"
#include "audio.h"

/** @file	entity.h
 *	@brief	Entity system with resource management and functions to do all: draw, touch, update, and think functions
 */


/**
 * @enum ENTITY_TYPES
 * @brief the different entities that could be created, seperated by file
 */
typedef enum
{
	CAMERA = 0,
	PLAYER,
	ENEMY_CLARENCE,
	ENEMY_MELT,
	ENEMY_MILK,
	ENEMY_CELERY,
	ENEMY_PROFESSOR,
	POWER_UP_GOO,
	POWER_UP_SHIELD,
	POWER_UP_SPREAD,
	POWER_UP_DOUBLE,
	POWER_UP_LIFE,
	POWER_UP_BOMB,
	PROJECTILE_PEP_SPREAD,
	PROJECTILE_PEP_CHARGE,
	PROJECTILE_PEP_BOMB,
	PROJECTILE_MELT,
	PROJECTILE_PROFESSOR,
}ENTITY_TYPES;


/** 
 * @struct	Entity structure
 * @brief	anything that can act or be acted upon (Enemies, players, power_ups, weapons, etc.) 
 */
typedef struct Entity_s
{
	Uint8 alive;													/**< flag to know if the entity is currently active */
	Uint32 id;														/**< id of the entity to find it */

	Vect2d position;												/**< x and y coordinates of the entity */
	Vect2d velocity;												/**< x and y velocities that the entity is moving */
	Vect2d direction;												/**< normalized vector of which way the entity will be moving */
	SDL_Rect bounds;												/**< the bounding box of the entity, determined by the sprite */
	
	Sprite *sprite;													/**< sprite component of the entity */
	int frameNumber;												/**< frame number the entity is on */

	SoundPak *entitySounds;											/**< The entity's sound files*/

	int health;														/**< the current health of the entity */
	Uint8  state;													/**< special states for each entity type (ie: stickied enemy, shielded player, goo bullet)*/
	Uint8  bulletState;												/**< the state an entities bullets will be in*/

	struct Entity_s *owner;											/**< the owner of the entity, such as the entity that fired the bullet */
	struct Entity_s *target;										/**< the target of this struct (ie: entity will be chasing player, power_up gives player a shield) */

	Uint32 nextThink;												/**< the next moment the entity will think */
	Uint32 thinkRate;												/**< how long the entity will wait to think again */

	void (*draw)(Sprite *sprite,int frame, Vect2d drawPos);			/**< draw function for the entity, most if not all entities will just be using sprite_draw */
	void (*think) (struct Entity_s *self);							/**< behavior of the entity */
	void (*update) (struct Entity_s *self);							/**< update function for changing the entity's data and checking its status */
	void (*touch) (struct Entity_s *self, struct Entity_s *other);	/**< collision behavior of the entity */
	void (*free) (struct Entity_s *self);							/**< function to free the entity */

}Entity;

/**
 * @brief	Frees the entity from memory and then frees the pointer to that entity.
 * @param [in,out]	double pointer to the entity.
 */
void entity_free(Entity **entity);

/**
 * @brief	loads a new entity's config data (excluding sprite info and bounds)
 * @param	nextThink		the next time the entity will think
 * @param	thinkRate		the rate at which the entity will think
 * @param	health		  	the starting health of this entity
 * @param	position		2D vector of where the entity will be in the game world
 * @param	velocity		2D vector of how fast the entity could move at max in both directions
 * @return [out] entity		If non-null, the entity.
 */
Entity *entity_new(Uint32 nextThink, Uint32 thinkRate, int health, Vect2d position, Vect2d velocity);

/** 
 *  @brief	closes the entity system by freeing the entire entityList and setting the number   
 *	   		 of entities in the list to 0. 
 */
void entity_close_system();

/**
 * @brief	Initializes the entity system by allocating space for the entityList.
 * @param	maxEntity	the maximum number of entities for the game saved in a static variable for later use.
 */
void entity_initialize_system(int maxEntity);

/** @brief	go through all the entities and have them think if they have a think function. */
void entity_think_all();

/** @brief	go through entity list and if the entity is inuse and has an update then run that update function. */
void entity_update_all();

/** @brief	go through entity list and draw them if they are in the camera's bounds. */
void entity_draw_all();

/** 
 *  @brief	draws an entity to the screen using sprite_draw   
 *  @param	entity	the entity in question
 */
void entity_draw(Entity *entity);

/**
 * @brief	go through all active entities checking if the given entity is colliding with any others, perfroming touch functions if either entity has them.
 * @param [in,out]	entity	If non-null, the Entity to process.
 */
void entity_intersect_all(Entity *entity);

/**
 * @brief	checks if the two given entities are inside each other's bounding box's using rect_intersect defined in vector.h.
 * @param [in]	entity	If non-null, the Entity to check for.
 * @param [in]	other	If non-null, the Entity to check against.
 * @return	1 if the entities are intersecting, else 0.
 */
int entity_intersect(Entity *entity, Entity *other);

/**
 * @brief	returns the entity with the specified id.
 * @param	id	The identifier.
 * @return	null if it fails, else an Entity pointer with the specified id.
 */
Entity *entity_get_by_id(Uint32 id);



#endif