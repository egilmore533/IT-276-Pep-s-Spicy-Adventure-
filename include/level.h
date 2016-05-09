#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "entity.h"

/** @file	level.h
 *	@brief	level storage, loading and config parsing, most important function being entity_load
 */

/**
 * @struct Level
 * @brief used to check if the end of the level has been reached by the player
 */
typedef struct {
	BackgroundPak *background;			/**< BackgroundPak */
	Music *backgroundMusic;				/**< backgroundMusic for the level */
	Entity *cam;						/**< the game's camera entity */
	Entity *player;						/**< the game's player entity */
	char *level_def_file;				/**< the def file for this level */
	int end;							/**< the x corrdinate of the end of the level */
}Level;

/**
 * @brief determines which entity type to load, loads everything for that entity (excluding position), and sets the id, then returns the new entity so its position can be set
 * @param type	the type of entity to load (enumerated in entity.h)
 * @param id	the id number of the entity to be loaded
 * @return	the new entity
 */
Entity *level_entity_load(int type, int id);

/** 
 * @brief frees the level from memory
 * @param [in,out]	level	double pointer that will be freed
 */
void level_free(Level **level);

/**
 * @brief parses the def file for what music, BackgroundPak def file, end flag, and entities to load, saving the player, entity, end flag, music, and backgroundPak and the level def file's name
 * @param	[in]	level_def_file	the level's def file
 * @return	the fully loaded level
 */
Level *level_load(char *level_def_file);

/**
 * @brief creates a level file using cJSON and the actorList saving the number of the type of actor and the positions of all the entities of that type, default background, music, camea and player are saved
 *			gets to the type of entity it is going to save next, checks how many actors there are in the actorList, the loops through the actorList searching for their position
 */
void level_save();

/**
 * @brief checks if the player is past the end flag for the level
 * @return 1 if player past the end, else 0
 */
Uint8 level_end_reached(Level *level);

#endif
