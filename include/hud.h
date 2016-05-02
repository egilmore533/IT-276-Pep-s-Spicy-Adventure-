#ifndef __HUD_H__
#define __HUD_H__

#include "entity.h"
#include "glib.h"

typedef struct 
{
	Entity	*player;
	Entity	*camera;
	Sprite	*background;

	Sprite	*lives;
	Sprite	*livesLabel;

	Sprite  *bombs;
	Sprite  *bombsLabel;

	Sprite	*pointsText;
	Sprite	*pointsLabel;
}HUD;


HUD *hud_initialize();

void hud_draw(HUD *hud);

#endif