#include "hud.h"
#include "SDL_ttf.h"
#include "player.h"
#include "camera.h"

TTF_Font	*hudFont = NULL;
SDL_Color	hudFontColor = {0, 0, 0};

HUD *hud_initialize()
{
	HUD *temp = NULL;

	hudFont = TTF_OpenFont("fonts/font1.ttf", 16);
	
	temp = (HUD *)malloc(sizeof(HUD));

	temp->player = player_get();
	temp->camera = camera_get();

	temp->background = sprite_load("images/hud_background_bar.png", vect2d_new(1366, 150), 1, 1);
	temp->lives = sprite_load("images/life2.png",vect2d_new(64, 96), 1, 1);
	temp->bombs = sprite_load("images/hud_bomb.png", vect2d_new(96, 96), 1, 1);
	
	temp->livesLabel = sprite_load_text(hudFont, "Lives", hudFontColor);
	temp->bombsLabel = sprite_load_text(hudFont, "Bombs", hudFontColor);
	return temp;
}

void hud_draw(HUD *hud)
{
	int i;
	Vect2d drawPos;
	float hud_height = hud->camera->position.y + HUD_HEIGHT;

	vect2d_set(drawPos, hud->camera->position.x, hud_height);

	sprite_draw(hud->background, 0, drawPos);
	
	drawPos.x = hud->camera->position.x + 160;
	drawPos.y = hud_height + 10 + hud->lives->frameSize.y;
	sprite_text_draw(hud->livesLabel, drawPos);

	drawPos.x = hud->camera->position.x + 525;
	drawPos.y = hud_height + 7 + hud->bombs->frameSize.y;
	sprite_text_draw(hud->bombsLabel, drawPos);

	vect2d_set(drawPos, hud->camera->position.x, hud_height);
	drawPos.y = hud_height + 15;
	for(i = 0; i < hud->player->inventory[LIVES]; i++)
	{
		drawPos.x = hud->camera->position.x + 11 + (11 * i) + hud->lives->frameSize.x * i;
		sprite_draw(hud->lives, 0, drawPos);
	}

	drawPos.y = hud_height + 15;
	for(i = 0; i < hud->player->inventory[BOMBS]; i++)
	{
		drawPos.x = hud->camera->position.x + 400 + (11 * i) + hud->bombs->frameSize.x * i;
		sprite_draw(hud->bombs, 0, drawPos);
	}
}