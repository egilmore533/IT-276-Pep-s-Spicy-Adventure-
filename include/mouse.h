#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "sprite.h"

typedef struct Mouse_s
{
	Uint8 alive;
	Vect2d position;
	SDL_Rect bounds;
	Sprite *sprite;
	int frameNumber;
}Mouse;

void mouse_initialize();

void mouse_update();

void mouse_free();

void mouse_draw();

Uint8 currentType_get();

Mouse *mouse_get();

void mouse_type_next();

void mouse_type_previous();

#endif
