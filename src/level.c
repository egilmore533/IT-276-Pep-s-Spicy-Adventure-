#include <stdio.h>
#include <stdlib.h>

#include "level.h"
#include "audio.h"

#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "power_up.h"

#include "cJSON.h"
#include "simple_logger.h"

Entity *level_entity_load(int type, int id)
{
	Entity *entity = NULL;
	switch(type)
	{
		case CAMERA:
			entity = camera_load(id);
			break;
		case PLAYER:
			entity = player_load();
			break;
		case ENEMY_CLARENCE:
			entity = enemy_load(ENEMY_CLARENCE, entity);
			break;
		case ENEMY_MELT:
			entity = enemy_load(ENEMY_MELT, entity);
			break;
		case ENEMY_MILK:
			entity = enemy_load(ENEMY_MILK, entity);
			break;
		case ENEMY_CELERY:
			entity = enemy_load(ENEMY_CELERY, entity);
			break;
		case ENEMY_PROFESSOR:
			entity = enemy_load(ENEMY_PROFESSOR, entity);
			break;
		case POWER_UP_GOO:
			entity = power_up_load(POWER_UP_GOO, entity);
			break;
		case POWER_UP_SHIELD:
			entity = power_up_load(POWER_UP_SHIELD, entity);
			break;
		case POWER_UP_SPREAD:
			entity = power_up_load(POWER_UP_SPREAD, entity);
			break;
		case POWER_UP_DOUBLE:
			entity = power_up_load(POWER_UP_DOUBLE, entity);
			break;
		case POWER_UP_LIFE:
			entity = power_up_load(POWER_UP_LIFE, entity);
			break;
		case POWER_UP_BOMB:
			entity = power_up_load(POWER_UP_BOMB, entity);
			break;
	}
	entity->id = id;
	return entity;
}