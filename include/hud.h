#ifndef __HUD_H__
#define __HUD_H__

#include "entity.h"
#include "glib.h"

typedef struct 
{
	Entity	*player;
	Sprite	*background;
	Sprite	*lives;
	Sprite * bombs;
	Vect2d	livesPosition;
}HUD;


HUD *hud_initialize();

void hud_draw(HUD *hud);

#endif