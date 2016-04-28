#include <stdio.h>
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

#define	MAX_SPRITES		1000

void initialize_all_systems();
void clean_up_all();
void initialize_next_level(Uint8 level_number);
void arcade_mode();

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;

	initialize_all_systems();

	//init main menu
	//loop until player selects next game mode
	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			done = 1;
		}
		if(keys[SDL_SCANCODE_A])
		{
			arcade_mode();
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
}

void initialize_all_systems()
{
	init_logger(LOG_FILE); //init simple logger from DJ's source code
	
	graphics_initialize("Pep's Spicy Adventure", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	audio_initialize(128, 3, 30);

	sprite_initialize_system(MAX_SPRITES);
	particle_initialize_system(2000);
	entity_initialize_system(200);

	background_initialize_system(8);
}

void arcade_mode()
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
				break;
			}
			else
			{
				level = level_load(level_path);
			}
		}
		if(level->player->state == GAME_OVER_STATE)
		{
			player_saved_load_off();
			level_purge_systems();
			level_free(&level);
			done = 1;
		}
		background_update_all();
		background_draw_all();

		entity_think_all();
		entity_update_all();
		entity_draw_all();

		particle_think_all();
		particle_check_all_dead();
		particle_draw_all();
		
		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			done = 1;
		}
	}while(!done);
	player_saved_load_off();
}