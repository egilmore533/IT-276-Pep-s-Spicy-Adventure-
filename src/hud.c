#include "hud.h"
#include "SDL_ttf.h"
#include "player.h"
#include "camera.h"
#include "simple_logger.h"

HUD *hud = NULL;
TTF_Font	*hudFont = NULL;
SDL_Color	hudFontColor = {0, 0, 0};

void hud_initialize()
{
	hudFont = TTF_OpenFont("fonts/HussarPrintASpicyAdventure.ttf", 16);
	
	hud = (HUD *)malloc(sizeof(HUD));
	memset(hud, 0, sizeof (HUD));

	hud->player = player_get();
	hud->camera = camera_get();

	hud->background = sprite_load("images/hud_background_bar.png", vect2d_new(1366, 150), 1, 1);
	hud->lives = sprite_load("images/life2.png",vect2d_new(64, 96), 1, 1);
	hud->bombs = sprite_load("images/hud_bomb.png", vect2d_new(96, 96), 1, 1);
	
	hud->livesLabel = sprite_load_text(hudFont, "Lives", hudFontColor);
	hud->bombsLabel = sprite_load_text(hudFont, "Bombs", hudFontColor);
}

void hud_draw()
{
	int i;
	Vect2d drawPos;
	float hud_height = hud->camera->position.y + HUD_HEIGHT;

	vect2d_set(drawPos, hud->camera->position.x, hud_height);

	sprite_draw(hud->background, 0, drawPos);
	
	drawPos.x = hud->camera->position.x + 160;
	drawPos.y = hud_height + 20 + hud->lives->frameSize.y;
	sprite_text_draw(hud->livesLabel, drawPos);

	drawPos.x = hud->camera->position.x + 525;
	drawPos.y = hud_height + 20 + hud->bombs->frameSize.y;
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

void hud_free()
{
	if(!hud)
	{
		slog("hud not initialized");
		return;
	}

	if(hud->player)
	{
		hud->player = NULL;
	}
	if(hud->camera)
	{
		hud->camera = NULL;
	}
	
	if(hud->background)
	{
		sprite_free(&hud->background);
	}

	if(hud->lives)
	{
		sprite_free(&hud->lives);
	}
	if(hud->livesLabel)
	{
		sprite_free(&hud->livesLabel);
	}

	if(hud->bombs)
	{
		sprite_free(&hud->bombs);
	}
	if(hud->bombsLabel)
	{
		sprite_free(&hud->bombsLabel);
	}

	if(hud->pointsText)
	{
		sprite_free(&hud->pointsText);
	}
	if(hud->pointsLabel)
	{
		sprite_free(&hud->pointsLabel);
	}
}

HUD *hud_get()
{
	if(hud)
		return hud;
	else
		return NULL;
}