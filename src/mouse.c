#include "mouse.h"
#include "camera.h"
#include "actor.h"
#include "simple_logger.h"

Mouse *mouse = NULL;
Uint8 currentType = ENEMY_CLARENCE;

void mouse_initialize()
{
	mouse = (Mouse *)malloc(sizeof(Mouse));
	memset(mouse, 0, sizeof (Mouse));
	mouse->alive = 1;
	mouse->bounds = rect(0, 0, 1, 1);
	mouse->frameNumber = 0;
}

void mouse_update()
{
	int mouseX, mouseY;
	Vect2d mousePosition;
	SDL_Event click_event;
	Entity *cam = camera_get();
	Actor *actor = NULL;
	
	SDL_GetMouseState(&mouseX, &mouseY);
	mousePosition = vect2d_new(mouseX, mouseY);

	if(cam)
	{
		mousePosition.x += cam->position.x;
		mousePosition.y += cam->position.y;
	}

	mouse->position = mousePosition;

	SDL_PollEvent(&click_event);
	if(click_event.type == SDL_MOUSEBUTTONDOWN)
	{
		actor = actor_new(mouse->position, currentType);
	}

	/*if(mouse->frameNumber < mouse->sprite->frames)
	{
		mouse->frameNumber++;
	}
	else
	{
		mouse->frameNumber = 0;
	}
	
	mouse_draw();*/
}

void mouse_free()
{
	if(!mouse)
	{
		slog("mouse not initialized");
		return;
	}

	if(mouse->sprite)
	{
		sprite_free(&mouse->sprite);
	}
}

void mouse_draw()
{
	sprite_draw(mouse->sprite, mouse->frameNumber, mouse->position);
}

Mouse *mouse_get()
{
	return mouse;
}

Uint8 currentType_get()
{
	return currentType;
}

void mouse_type_next()
{
	if(currentType < POWER_UP_BOMB)
	{
		currentType++;
	}
	else
	{
		currentType = ENEMY_CLARENCE;
	}
}

void mouse_type_previous()
{
	if(currentType > ENEMY_CLARENCE)
	{
		currentType--;
	}
	else
	{
		currentType = POWER_UP_BOMB;
	}
}