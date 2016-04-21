#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "entity.h"

/** @file	level.h
 *	@brief	level storage, loading and config parsing, most important function being entity_load
 */


/**
 * @brief parses the entity_config file for data to load each entity from then sets the entity up accordingly, needs to be given a type, position and an id
 * @param type	the type of entity to load (enumerated in entity.h)
 * @param pos	the 2d vector of the entity's position
 * @param id	the id number of the entity to be loaded
 */
Entity *entity_load(int type, Vect2d pos, int id);

#endif
