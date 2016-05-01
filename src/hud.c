#include "hud.h"
#include "player.h"
#include "camera.h"

HUD *hud_initialize()
{
	HUD *temp = NULL;
	
	temp = (HUD *)malloc(sizeof(HUD));
	temp->player = player_get();
	temp->background = sprite_load("images/hud_background_bar.png", vect2d_new(1366, 150), 1, 1);
	temp->lives = sprite_load("images/life2.png",vect2d_new(64, 96), 1, 1);
	temp->bombs = sprite_load("images/hud_bomb.png", vect2d_new(96, 96), 1, 1);
	temp->livesPosition = vect2d_new(100, 50);

	return temp;
}

void hud_draw(HUD *hud)
{
	int i;
	Vect2d drawPos;
	Entity *cam = camera_get();

	vect2d_set(drawPos, cam->position.x, cam->position.y + HUD_HEIGHT);

	sprite_draw(hud->background, 0, drawPos);
	drawPos.y = drawPos.y + 15;
	for(i = 0; i < hud->player->inventory[LIVES]; i++)
	{
		drawPos.x = cam->position.x + 11 + (11 * i) + hud->lives->frameSize.x * i;
		sprite_draw(hud->lives, 0, drawPos);
	}

	for(i = 0; i < hud->player->inventory[BOMBS]; i++)
	{
		drawPos.x = cam->position.x + 400 + (11 * i) + hud->bombs->frameSize.x * i;
		sprite_draw(hud->bombs, 0, drawPos);
	}
}