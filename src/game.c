#include <stdio.h>
#include "graphics.h"
#include "audio.h"
#include "simple_logger.h"
#include "sprite.h"
#include "camera.h"
#include "background.h"
#include "particle.h"
#include "level.h"

#define	MAX_SPRITES		1000

void initialize_all(Uint8 level_number);
void clean_up_all();
void initialize_next_level(Uint8 level_number);

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;

	//test start
	Vect2d test_frame_size = vect2d_new(128, 64);
	Sprite *test = NULL;
	Music *background_music = NULL;
	Vect2d camera_pos = vect2d_new(0, 0);
	Vect2d player_pos = vect2d_new(200, 200);
	Vect2d test_pos = vect2d_new(1220, 200);
	Vect2d test_pos2 = vect2d_new(1220, 500);
	Entity *test_ent;
	Entity *test_ent2;
	Entity *test_player;
	Entity *cam;
	//test end

	initialize_all(1);
	cam = camera_get();
	cam->position = camera_pos;
	the_renderer = graphics_get_renderer();

	//test start
	background_music = audio_load_music("sound/HotSalsa.ogg", -1);
	audio_play_music(background_music); 
	test = sprite_load("images/pep3.png", test_frame_size, 2, 2);
	background_pak_new("def/sunny_peps_background_config.txt");
	test_player = level_entity_load(PLAYER, 1);
	test_player->position = player_pos;
	test_ent = level_entity_load(ENEMY_MELT, 2);
	test_ent->position = test_pos;
	test_ent2 = level_entity_load(ENEMY_MELT, 3);
	test_ent2->position = test_pos2;
	//test end

	done = 0;
	do
	{
		SDL_RenderClear(the_renderer);
		background_update_all();
		background_draw_all();

		entity_think_all();
		entity_update_all();
		entity_draw_all();

		particle_think_all();
		particle_draw_all();
		
		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			done = 1;
		}
	}while(!done);

	slog("\n\n QUIT GAME \n\n");

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

void initialize_all(Uint8 level_number)
{
	init_logger("text/log.txt"); //init simple logger from DJ's source code
	
	graphics_initialize("Pep's Spicy Adventure", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	audio_initialize(128, 3, 30);

	sprite_initialize_system(MAX_SPRITES);
	particle_initialize_system(2000);
	entity_initialize_system(200);
	level_entity_load(CAMERA, 0); //test

	background_initialize_system(8);
}

void initialize_next_level(Uint8 level_number)
{
	
}
