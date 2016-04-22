#ifndef __POWER_UP_H__
#define __POWER_UP_H__

#include "entity.h"

Entity *power_up_load(int type, Entity *power_up);

void power_up_update(Entity *power_up);

void power_up_heat_shield_touch(Entity *heat_shield, Entity *other);

void power_up_goo_shot_touch(Entity *goo_shot, Entity *other);

void power_up_spread_shot_touch(Entity *spread_shot, Entity *other);

void power_up_double_tap_touch(Entity *double_tap, Entity *other);

void power_up_extra_life_touch(Entity *extra_life, Entity *other);

void power_up_bomb_pickup_touch(Entity *bomb_pickup, Entity *other);

#endif