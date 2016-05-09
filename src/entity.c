#include "simple_logger.h"

#include "entity.h"


/* entity resource manager */
static Entity	*entityList = NULL;
static int		entityNum = 0;
static int		entityMax = 0;

void entity_free(Entity **entity)
{
	Entity *self;
	if(!entity)
	{
		return;
	}
	else if(!*entity) 
	{
		return;
	}
	self = *entity;
	self->alive--;

	if(self->sprite != NULL)
	{
		sprite_free(&self->sprite);
	}
	if(self->entitySounds != NULL)
	{
		audio_pak_free(&self->entitySounds);
	}
	entityNum--;
	*entity = NULL;
}

Entity *entity_new(Uint32 thinkRate, int health, Vect2d velocity)
{
	int i;
	/*makesure we have the room for a new sprite*/
	if(entityNum + 1 > entityMax)
	{
		slog("Maximum Entity Reached.");
		exit(1);
	}
	for(i = 0; i < entityMax; i++)
	{
		if(entityList[i].alive)
		{
			continue;
		}

		//found the first entity in the list that is free to use, 
		// clear memory, fill out config data, increment entityNum, and return the new entity
		memset(&entityList[i],0,sizeof(Entity));

		entityList[i].alive = 1;
		entityList[i].thinkRate = thinkRate;
		entityList[i].health = health;
		entityList[i].maxVelocity = velocity;
		entityList[i].free = &entity_free; // this is the default free function
		entityNum++;

		return &entityList[i];
	}
	return NULL;
}

void entity_close_system()
{
	int i;
	Entity *entity;
	if(!entityList)
	{
		slog("entityList not initialized");
		return;
	}
	for(i = 0; i < entityMax; i++)
	{
		entity = &entityList[i];
		entity_free(&entity);
	}
	entityNum = 0;
	free(entityList);
	entityList = NULL;
}

void entity_initialize_system(int maxEntity)
{
	int i;
	if(maxEntity == 0)
	{
		slog("Max entity == 0");
		return;
	}
	entityList = (Entity *) malloc (sizeof (Entity) * maxEntity);
	if(!entityList)
	{
		slog("failed to initialize entityList");
		return;
	}
	memset(entityList, 0, sizeof (Entity) * maxEntity);
	entityNum = 0;
	for(i = 0; i < maxEntity; i++)
	{
		entityList[i].sprite = NULL;
		entityList[i].owner = NULL;
		entityList[i].target = NULL;
	}
	entityMax = maxEntity;
	atexit(entity_close_system);
}

void entity_think_all()
{
	int i;
	for(i = 0; i < entityMax; i++)
	{
		if(!entityList[i].alive)
		{
			continue;
		}
		if(!entityList[i].think)
		{
			continue;
		}

		entityList[i].think(&entityList[i]);
	}
}

void entity_update_all()
{
	int i;
	for(i = 0; i < entityMax; i++)
	{
		if(!entityList[i].alive)
		{
			continue;
		}
		if(!entityList[i].update)
		{
			continue;
		}

		entityList[i].update(&entityList[i]);
	}
}

void entity_draw_all()
{
	int i;
	Entity *cam = NULL; //get the camera
	for(i = 0; i < entityMax; i++)
	{
		if(!entityList[i].alive)
		{
			continue;
		}
		if(!entityList[i].draw)
		{
			continue;
		}
		entityList[i].draw(entityList[i].sprite, entityList[i].frameNumber, entityList[i].position);
	}
}

void entity_draw(Entity *entity)
{
	if(!entity)
	{
		slog("No entity to draw");
		return;
	}
	
	//draw the entity to the position on the entity as if the camera was the actual screen
	entity->draw(entity->sprite, entity->frameNumber, entity->position);
}

void entity_intersect_all(Entity *entity)
{
	int i;
	if(!entity)
	{
		return;
	}
	for(i = 0; i  < entityMax; i++)
	{
		if(!entityList[i].alive)
		{
			continue;
		}
		/* don't check yourself*/
		if(entity == &entityList[i])
		{
			continue;
		}
		
		if(entity_intersect(entity, &entityList[i]))
		{
			if(entity->touch)
			{
				entity->touch(entity, &entityList[i]);
			}
			//in case the entity touches something and frees itself
			if(!entity->alive)
			{
				return;
			}
			if(entityList[i].touch)
			{
				entityList[i].touch(&entityList[i], entity);
			}
		}
	}
	return;
}

int entity_intersect(Entity *entity, Entity *other)
{
	SDL_Rect aB, bB;
	if(!entity || !other)
	{
		slog("entity error");
		return 0;
	}
	aB = rect(entity->position.x + entity->bounds.x,
				entity->position.y + entity->bounds.y,
				entity->bounds.w,
				entity->bounds.h
		);
	bB = rect(other->position.x + other->bounds.x,
				other->position.y + other->bounds.y,
				other->bounds.w,
				other->bounds.h
		);
	return rect_intersect(aB, bB);
}

Entity *entity_get_by_id(Uint32 id)
{
	int i;
	for (i = 0; i < entityMax; i++)
	{
		if(entityList[i].alive == 0)
		{
			continue;
		}
		if(entityList[i].id == id)
		{
			return &entityList[i];
		}
	}
	return NULL;
}

void entity_empty_list()
{
	int i;
	Entity *entity = NULL;
	if(!entityList)
	{
		slog("entityList not initialized");
		return;
	}
	for(i = 0; i < entityMax; ++i)
	{
		entity = &entityList[i];
		entity_free(&entity);
	}
	memset(entityList, 0, sizeof(Entity) * entityMax);
	for(i = 0; i < entityMax; i++)
	{
		entityList[i].sprite = NULL;
		entityList[i].owner = NULL;
		entityList[i].target = NULL;
	}
	entityNum = 0;
}
