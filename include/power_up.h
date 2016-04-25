#ifndef __POWER_UP_H__
#define __POWER_UP_H__

#include "entity.h"

/** @file	power_up.h
 *	@brief	power_ups that target the player and boosts their abilities
 */

/**
 * @brief used to parse the def file (based on the type entered sets the corresponding touch function) for the power up info and returns the loaded entity
 * @param type	the type of power_up that is being loaded
 * @param [in,out]	the power_up
 * @return	the power_up that was just loaded
 */
Entity *power_up_load(int type, Entity *power_up);

/** 
 *  @brief  basic update that only calls intersect all for the power_up
 *  @param  power_up  	the power_up entity that is being checked for collisions
 */
void power_up_update(Entity *power_up);

/**
 *  @brief  checks if the thing the heat_shield is touching its target (the player) and acts accordingly
 *  @param  heat_shield the heat_shield entity
 *  @param  other		the thing heat_shield is touching
 */
void power_up_heat_shield_touch(Entity *heat_shield, Entity *other);

/**
 *  @brief  checks if the thing the goo_shot is touching its target (the player) and acts accordingly
 *  @param  goo_shot 		the goo_shot entity
 *  @param  other			the thing goo_shot is touching
 */
void power_up_goo_shot_touch(Entity *goo_shot, Entity *other);

/**
 *  @brief  checks if the thing the spread_shot is touching its target (the player) and acts accordingly
 *  @param  spread_shot 		the spread_shot entity
 *  @param  other				the thing spread is touching
 */
void power_up_spread_shot_touch(Entity *spread_shot, Entity *other);

/**
 *  @brief  checks if the thing the double_tap is touching its target (the player) and acts accordingly
 *  @param  double_tap  the double_tap entity
 *  @param  other		the thing double_tap is touching
 */
void power_up_double_tap_touch(Entity *double_tap, Entity *other);

/**
 *  @brief  checks if the thing the extra_life is touching its target (the player) and acts accordingly
 *  @param  spread 		the extra_life entity
 *  @param  other		the thing extra_life is touching
 */
void power_up_extra_life_touch(Entity *extra_life, Entity *other);

/**
 *  @brief  checks if the thing the bomb_pickup is touching its target (the player) and acts accordingly
 *  @param  bomb_pickup 		the bomb_pickup entity
 *  @param  other				the thing bomb_pickup is touching
 */
void power_up_bomb_pickup_touch(Entity *bomb_pickup, Entity *other);

#endif