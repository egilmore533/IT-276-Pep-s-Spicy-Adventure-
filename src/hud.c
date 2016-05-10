#include <stdio.h>
#include <stdlib.h>

#include "SDL_ttf.h"

#include "simple_logger.h"

#include "hud.h"
#include "actor.h"
#include "button.h"
#include "files.h"
#include "mouse.h"
#include "player.h"


/* hud management (used for arcade, endless, challenge and editor mode) */
static HUD			*hud = NULL;

/* normal hud text data (not used for editor mode) */
static TTF_Font		*hudFont = NULL;
static SDL_Color	hudFontColor = {0, 0, 0};

/* normal hud's grade data (colors need to be changed dynamically so they are defined here) */
static TTF_Font		*hudGradeFont = NULL;
static SDL_Color	hudGradeColor_F = {247, 55, 8};
static SDL_Color	hudGradeColor_D = {247, 132, 26};
static SDL_Color	hudGradeColor_C = {0, 247, 49};
static SDL_Color	hudGradeColor_B = {0, 126, 247};
static SDL_Color	hudGradeColor_A = {239, 247, 0};
static SDL_Color	hudGradeColor_S = {247, 0, 167};
static SDL_Color	hudGradeColor_SS = {247, 231, 0};
static SDL_Color	hudGradeColor_SSS = {247, 247, 247};

/* editor hud buttons */
static Button		*nextButton = NULL;
static Button		*prevButton = NULL;

void hud_initialize()
{
	char temp[100];
	char *tempGrade;
	Uint8 multiplier;
	static SDL_Color gradeColor;
	hudFont = TTF_OpenFont(PEP_FONT, 16);
	hudGradeFont = TTF_OpenFont(PEP_FONT, 40);
	
	hud = (HUD *)malloc(sizeof(HUD));
	memset(hud, 0, sizeof (HUD));

	hud->player = player_get();
	hud->camera = camera_get();

	hud->background = sprite_load("images/hud_background_bar.png", vect2d_new(1366, 150), 1, 1);
	hud->lives = sprite_load("images/life2.png",vect2d_new(64, 96), 1, 1);
	hud->bombs = sprite_load("images/hud_bomb.png", vect2d_new(96, 96), 1, 1);
	
	hud->livesLabel = sprite_load_text(hudFont, "Lives", hudFontColor);
	hud->bombsLabel = sprite_load_text(hudFont, "Bombs", hudFontColor);

	sprintf(temp, "Score: %d", hud->player->points);
	hud->pointsLabel = sprite_load_text(hudFont, temp, hudFontColor);

	multiplier = get_multiplier();
	switch(multiplier)
	{
	case 1: 
		tempGrade = "Multiplier: F";
		gradeColor = hudGradeColor_F;
		break;
	case 2: 
		tempGrade = "Multiplier: D";
		gradeColor = hudGradeColor_D;
		break;
	case 3:
		tempGrade = "Multiplier: C";
		gradeColor = hudGradeColor_C;
		break;
	case 4:
		tempGrade = "Multiplier: B";
		gradeColor = hudGradeColor_B;
		break;
	case 5:
		tempGrade = "Multiplier: A";
		gradeColor = hudGradeColor_A;
		break;
	case 6:
		tempGrade = "Multiplier: S";
		gradeColor = hudGradeColor_S;
		break;
	case 7:
		tempGrade = "Multiplier: SS";
		gradeColor = hudGradeColor_SS;
		break;
	case 8:
		tempGrade = "Multiplier: SSS";
		gradeColor = hudGradeColor_SSS;
		break;
	}
	hud->pointsMultiplierLabel = sprite_load_text(hudGradeFont, tempGrade, gradeColor);
}

void hud_draw()
{
	static Uint32 points = 0;
	static Uint8 multiplier = 1;
	char temp[100];
	char *tempGrade = "F";
	static Uint8 multiplierOld;
	static SDL_Color gradeColor;
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

	drawPos.x = hud->camera->position.x + 950;
	drawPos.y = hud_height + 20;
	if(points != hud->player->points)
	{
		points = hud->player->points;
		sprintf(temp, "Score: %d", points);
		sprite_free(&hud->pointsLabel);
		hud->pointsLabel = sprite_load_text(hudFont, temp, hudFontColor);
	}
	sprite_text_draw(hud->pointsLabel, drawPos);

	if(multiplierOld != get_multiplier())
	{
		multiplierOld = get_multiplier();
		switch(multiplierOld)
		{
		case 1: 
			tempGrade = "Multiplier: F";
			gradeColor = hudGradeColor_F;
			break;
		case 2: 
			tempGrade = "Multiplier: D";
			gradeColor = hudGradeColor_D;
			break;
		case 3:
			tempGrade = "Multiplier: C";
			gradeColor = hudGradeColor_C;
			break;
		case 4:
			tempGrade = "Multiplier: B";
			gradeColor = hudGradeColor_B;
			break;
		case 5:
			tempGrade = "Multiplier: A";
			gradeColor = hudGradeColor_A;
			break;
		case 6:
			tempGrade = "Multiplier: S";
			gradeColor = hudGradeColor_S;
			break;
		case 7:
			tempGrade = "Multiplier: SS";
			gradeColor = hudGradeColor_SS;
			break;
		case 8:
			tempGrade = "Multiplier: SSS";
			gradeColor = hudGradeColor_SSS;
			break;
		}
		sprite_free(&hud->pointsMultiplierLabel);
		hud->pointsMultiplierLabel = sprite_load_text(hudGradeFont, tempGrade , gradeColor);
	}
	
	drawPos.x = hud->camera->position.x + 900;
	drawPos.y = hud_height + 75;
	sprite_text_draw(hud->pointsMultiplierLabel, drawPos);
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

	if(hud->pointsLabel)
	{
		sprite_free(&hud->pointsLabel);
	}
	if(hud->pointsMultiplierLabel)
	{
		sprite_free(&hud->pointsMultiplierLabel);
	}
	if(hudFont)
	{
		TTF_CloseFont( hudFont );
	}
	if(hudGradeFont)
	{
		TTF_CloseFont( hudGradeFont );
	}
}

HUD *hud_get()
{
	if(hud)
		return hud;
	else
		return NULL;
}

void hud_editor_initialize()
{
	hudFont = TTF_OpenFont(PEP_FONT, 16);
	hudGradeFont = NULL;

	hud = (HUD *)malloc(sizeof(HUD));
	memset(hud, 0, sizeof (HUD));

	hud->camera = camera_get();

	hud->background = sprite_load("images/hud_background_bar.png", vect2d_new(1366, 150), 1, 1);

	nextButton = button_load_small(vect2d_new(NEXT_BUTTON_X + hud->camera->position.x, NEXT_BUTTON_Y + hud->camera->position.y), "Next");
	nextButton->click = &mouse_type_next;

	prevButton = button_load_small(vect2d_new(PREV_BUTTON_X + hud->camera->position.x, PREV_BUTTON_Y + hud->camera->position.y), "Prev");
	prevButton->click = &mouse_type_previous;
	
	
}

void hud_editor_draw()
{
	static int currentType = 0;
	Vect2d drawPos;
	float hud_height = hud->camera->position.y + HUD_HEIGHT;

	vect2d_set(drawPos, hud->camera->position.x, hud_height);

	sprite_draw(hud->background, 0, drawPos);

	nextButton->position.x = hud->camera->position.x + NEXT_BUTTON_X;
	prevButton->position.x = hud->camera->position.x + PREV_BUTTON_X;

	nextButton->draw(nextButton);
	prevButton->draw(prevButton);

	if(currentType != currentType_get() || !hud->bombs)
	{
		currentType = currentType_get();
		hud->bombs = actor_load_sprite(currentType);
	}

	drawPos.x = hud->camera->position.x + 500;
	drawPos.y = hud_height + 15;
	sprite_draw(hud->bombs, 0 , drawPos);

	button_update(nextButton);
	button_update(prevButton);
}

void hud_editor_free()
{
	hud_free();
	button_free(&nextButton);
	button_free(&prevButton);
}
