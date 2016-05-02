#include <stdio.h>
#include <math.h>
#include "SDL_ttf.h"
#include "graphics.h"
#include "audio.h"
#include "simple_logger.h"
#include "sprite.h"
#include "camera.h"
#include "background.h"
#include "particle.h"
#include "level.h"
#include "files.h"
#include "player.h"
#include "hud.h"

#define	MAX_SPRITES		1000

HUD *hud;

TTF_Font *font;
SDL_Color textColor = {255, 210 ,4};

void initialize_all_systems();
void clean_up_all();
void initialize_next_level(Uint8 level_number);
void main_menu();
Uint8 arcade_mode();
void editor_mode();

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	Sprite *sprite = NULL;
	Sprite *text_start_menu = NULL;
	initialize_all_systems();

	//init title card, loop until player enters main menu
	sprite = sprite_load("images/Title.png", vect2d_new(1366, 768), 1, 1);
	text_start_menu = sprite_load_text(font, "Press Space to Start", textColor);

	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		sprite_draw(sprite, 0, vect2d_new(0, 0));
		sprite_text_draw(text_start_menu, vect2d_new(550, 500));
		SDL_SetTextureAlphaMod(text_start_menu->image, 100 * (1 + sin(SDL_GetTicks() * 2 * 3.14 / 1000)));

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_SPACE])
		{
			main_menu();
			if(!sprite)
			{
				sprite = sprite_load("images/Title.png", vect2d_new(1366, 768), 1, 1);
			}
			if(!text_start_menu)
			{
				text_start_menu = sprite_load_text(font, "Press Space to Start", textColor);
			}
		}
		else if(keys[SDL_SCANCODE_ESCAPE])
		{
			done = 1;
		}
	}while(!done);
	

	slog("QUIT GAME \n\n========================================================\n\n");

	exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
	return 0;
}

void clean_up_all()
{
	/* any other cleanup functions can be added here FILO order*/
	background_close_system();
	entity_close_system();
	sprite_close_system();
	audio_close_lists();
	graphics_close();
	TTF_CloseFont( font );
}

void initialize_all_systems()
{
	if(TTF_Init() == -1)
	{
		slog("error TTF_Init");
		exit(2);
	}
	font = TTF_OpenFont("fonts/font1.ttf", 28);

	init_logger(LOG_FILE); //init simple logger from DJ's source code
	
	graphics_initialize("Pep's Spicy Adventure", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	audio_initialize(128, 3, 30);

	sprite_initialize_system(MAX_SPRITES);
	particle_initialize_system(2000);
	entity_initialize_system(200);

	background_initialize_system(8);

}

Uint8 arcade_mode()
{
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;
	Uint8 level_num = 1;
	Level *level = NULL;
	char *level_path = NULL;

	level_purge_systems();
	the_renderer = graphics_get_renderer();
	level_path = files_get_level(level_num++);
	level = level_load(level_path);
	hud = hud_initialize();
	player_saved_load_on();
	done = 0;
	do
	{
		SDL_RenderClear(the_renderer);
		if(level_end_reached(level))
		{
			level_purge_systems();
			level_free(&level);
			level_path = files_get_level(level_num++);
			if(level_path == 0)
			{
				player_saved_load_off();
				return 1;//victory
			}
			else
			{
				level = level_load(level_path);
				hud = hud_initialize();
			}
		}
		if(level->player->state == GAME_OVER_STATE)
		{
			player_saved_load_off();
			level_purge_systems();
			level_free(&level);
			done = 1;
			return 0;//game over
		}
		background_update_all();
		background_draw_all();

		entity_think_all();
		entity_update_all();
		entity_draw_all();

		particle_think_all();
		particle_check_all_dead();
		particle_draw_all();

		hud_draw(hud);
		
		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			player_saved_load_off();
			level_purge_systems();
			done = 1;
			return 0;//game over
		}
	}while(!done);
	player_saved_load_off();
	return 0;
}

void editor_mode()
{

}


void main_menu()
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	Sprite *sprite = NULL;

	//init main menu
	//loop until player selects next game mode
	sprite = sprite_load("images/main_menu.png", vect2d_new(1366, 768), 1, 1);

	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);
		sprite_draw(sprite, 0, vect2d_new(0, 0));
		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_A])
		{
			if(arcade_mode())
			{
				//you win screen, go over points and look at high scores
				sprite = sprite_load("images/win_screen_message.png", vect2d_new(1366, 768), 1, 1);
			}
			else
			{
				//game over screen, go over points and look at high scores, try again button
				sprite = sprite_load("images/lose_screen_message.png", vect2d_new(1366, 768), 1, 1);
			}
		}
		else if(keys[SDL_SCANCODE_E])
		{
			editor_mode();
		}
		else if(keys[SDL_SCANCODE_ESCAPE])
		{
			done = 1;
		}
	}while(!done);
}

void title_text_draw(char *text)
{
	
}
