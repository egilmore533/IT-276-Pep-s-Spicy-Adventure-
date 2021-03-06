#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "entity.h"

/** @file	weapon.h
 *	@brief	attacks for entities, loaded using entity_load in level.h
 */

/**
 * @brief weapon fire is used to parse the def file for the weapon info and returns the loaded entity
 * @param type	the type of weapon that is being fired
 * @param owner	the owner of the weapon that is being fired
 * @return	the entity that was just loaded
 */
Entity *weapon_fire(int type, Entity *owner);

/**
 * @brief generic weapon think function
 * @param	the weapon thats thinking
 */
void weapon_think(Entity *weapon);

/**
 * @brief generic update function
 * @param	the weapon to be updated
 */
void weapon_update(Entity *weapon);

////////////////////PEP WEAPONS CODE////////////////////////

/**
 * @brief generic think for pep's shots, frees the shot after leaving camera bounds, and adds particles
 * @param [in,out]	spice	the shot that is being thunk
 */
void weapon_pep_think(Entity *spice);

/**
 * @brief pep's normal firing state, has spread shots if pep picked up spreads the logic for that uses some trig 
 *        to calculate the angle of the spread shots while always firing one shot foward
 * @param [in]	player		the player is used to calculate position of the bullet and also saved for later use
 */
void weapon_pep_spread_fire(Entity *player);

/**
 * @brief logic for spread bullets, always do one damage to enemies, they freeze when in the goo state
 * @param [in,out]	spice	the spice that collided with something
 * @param [in,out]	other	whatever collided with the spice
 */
void weapon_pep_spread_touch(Entity *spice, Entity *other);

/**
 * @brief creates a spread bullet (used in spread_fire to create quick bullets)
 * @param [in]	owner	the player who fired the bullet
 */
Entity *make_spread_bullet(Entity *owner);

/**
 * @brief pep's charge attack, this creates one large powerful shot that knocks pep back
 * @param [in]	player		the owner of the charge shot
 */
void weapon_pep_charge_fire(Entity *player);

/**
 * @brief generic think for pep's shots, frees the shot after leaving camera bounds, and adds particles that MOVE
 * @param [in,out]	spice	the shot that is being thunk
 */
void weapon_pep_charge_think(Entity *spice);

/**
 * @brief logic for charge bullets, always do 5 damage to enemies, they freeze when in the goo state
 * @param [in,out]	spice	the spice that collided with something
 * @param [in,out]	other	whatever collided with the spice
 */
void weapon_pep_charge_touch(Entity *spice, Entity *other);

/**
 * @brief pep's bomb attack, this clears the screen of all enemies
 * @param [in]	player		the owner of the bomb
 */
void weapon_pep_bomb(Entity *player);

/**
 * @brief logic to free bomb after being on screen for a moment
 */
void weapon_pep_bomb_think(Entity *bomb);

/**
 * @brief logic for bombs, always kills everything
 * @param [in,out]	bom	the bomb that collided with something
 * @param [in,out]	other	whatever collided with the bomb
 */
void weapon_pep_bomb_touch(Entity *bomb, Entity *other);


//////////////////////////MELT WEAPON CODE//////////////////////////////////


/**
 * @brief melts attack, uses pep's think to move
 * @param [in]	melt		the owner of the cream
 */
void weapon_melt_cream_fire(Entity *melt);

/**
 * @brief  checks if the thing the cream touched is the cream's target (the player) and hurts the target if so and frees itself
 * @param  cream			the cream
 * @param  other			what the cream collided with
 */
void weapon_melt_cream_touch(Entity *cream, Entity *other);


//////////////////////PROFESSOR SLICE WEAPON CODE///////////////////////////

/**
 * @brief professor slice's attack, moves for a random distance and sits still
 * @param [in]	professor_slice		the owner of the bread
 */
void weapon_professor_slice_bread_fire(Entity *professor_slice);

/**
 * @brief  checks if the thing the bread touched is the bread's target (the player) and hurts the target if so and frees itself
 * @param  bread			the bread
 * @param  other			what the bread collided with
 */
void weapon_professor_slice_bread_touch(Entity *bread, Entity *other);

/**
 * @brief slows the breads velocity, according to the think rate, until it stops
 * @param [in, out] bread			the bread
 */
void weapon_professor_slice_bread_think(Entity *bread);


#endif
