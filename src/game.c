#include <stdio.h>
#include <math.h>

#include "SDL_ttf.h"

#include "simple_logger.h"

#include "files.h"
#include "graphics.h"
#include "audio.h"
#include "sprite.h"

#include "hud.h"
#include "button.h"
#include "background.h"
#include "particle.h"
#include "level.h"

#include "camera.h"
#include "player.h"


#define	MAX_SPRITES		1000

TTF_Font *font;
SDL_Color textColor = {255, 210 ,4};

void initialize_all_systems();
void purge_systems();
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
		sprite_text_draw(text_start_menu, vect2d_new(550, 650));
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
	font = TTF_OpenFont("fonts/HussarPrintASpicyAdventure.ttf", 28);

	init_logger(LOG_FILE); //init simple logger from DJ's source code
	
	graphics_initialize("Pep's Spicy Adventure", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	audio_initialize(128, 3, 30);

	sprite_initialize_system(MAX_SPRITES);
	particle_initialize_system(2000);
	entity_initialize_system(200);

	background_initialize_system(8);

	button_initialize_system(10);

}

Uint8 arcade_mode()
{
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;
	Uint8 level_num = 1;
	Level *level = NULL;
	char *level_path = NULL;

	purge_systems();
	the_renderer = graphics_get_renderer();
	level_path = files_get_level(level_num++);
	level = level_load(level_path);
	hud_initialize();
	player_saved_load_on();
	done = 0;
	do
	{
		SDL_RenderClear(the_renderer);
		if(level_end_reached(level))
		{
			purge_systems();
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
				hud_initialize();
			}
		}
		if(level->player->state == GAME_OVER_STATE)
		{
			player_saved_load_off();
			purge_systems();
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

		hud_draw();
		
		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			player_saved_load_off();
			purge_systems();
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
	Button *arcadeButton = NULL;
	Sprite *sprite = NULL;

	//init main menu
	//loop until player selects next game mode
	arcadeButton = button_load_arcade_mode(vect2d_new(200, 200));
	arcadeButton->click = &arcade_mode;

	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		button_update_all();
		button_draw_all();

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_E])
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

void purge_systems()
{
	player_save_info();
	//button_empty_list();
	entity_empty_list();
	background_empty_list();
	audio_empty_list();
}
