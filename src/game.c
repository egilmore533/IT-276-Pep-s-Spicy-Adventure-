#include <stdio.h>
#include <math.h>

#include "SDL_ttf.h"

#include "simple_logger.h"

#include "files.h"
#include "graphics.h"
#include "audio.h"
#include "sprite.h"

#include "actor.h"
#include "mouse.h"

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

int	back = 0;

void initialize_all_systems();
void purge_systems();
void clean_up_all();
void initialize_next_level(Uint8 level_number);
void main_menu();
void arcade_mode();
void control_screen();
void editor_mode();
void back_question();
void yes_back();
void no_back();

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
	hud_free();
	button_close_system();
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

	button_initialize_system(20);

	actor_initialize_system(100);

}

void arcade_mode()
{
	int win = 0;
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
	
		if(level->player->state == GAME_OVER_STATE)
		{
			player_saved_load_off();
			purge_systems();
			level_free(&level);
			done = 1;
		}

		if(level_end_reached(level))
		{
			purge_systems();
			level_free(&level);
			level_path = files_get_level(level_num++);
			if(level_path == 0)
			{
				player_saved_load_off();
				win = 1;//victory
				done = 1;
			}
			else
			{
				level = level_load(level_path);
				hud_initialize();
			}
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
			back_question();
			if(back == 1)
			{
				player_saved_load_off();
				purge_systems();
				done = 1;
			}
			back = 0;
		}
	}while(!done);
	hud_free();
	if(win)
	{
		
	}
	else
	{
	
	}
}

void editor_mode()
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	mouse_initialize();
	hud_editor_initialize();
	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		actor_draw_all();

		hud_editor_draw();

		mouse_update();

		mouse_draw();

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			back_question();
			if(back == 1)
			{
				done = 1;
			}
			back = 0;
		}
	}while(!done);
	hud_editor_free();
	mouse_free();
	actor_empty_list();
}


void main_menu()
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	Button *arcadeButton = NULL;
	Button *controlButton = NULL;
	Button *editorButton = NULL;
	Sprite *sprite = NULL;

	camera_load(0);

	//init main menu
	//loop until player selects next game mode
	arcadeButton = button_load_arcade_mode(vect2d_new(200, 200));
	arcadeButton->click = &arcade_mode;

	controlButton = button_load_controls(vect2d_new(200, 400));
	controlButton->click = &control_screen;

	editorButton = button_load_editor_mode(vect2d_new(200, 600));
	editorButton->click = &editor_mode;

	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		button_update_all();
		button_draw_all();

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			back_question();
			if(back == 1)
			{
				done = 1;
			}
			back = 0;
		}
	}while(!done);
	button_empty_list();
}

void purge_systems()
{
	player_save_info();
	entity_empty_list();
	background_empty_list();
	audio_empty_list();
}

void control_screen()
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	Sprite *background = NULL;

	background = sprite_load("images/controls.png", vect2d_new(1366, 768), 1, 1);
	SDL_SetTextureAlphaMod(background->image, 100); //makes the screen fade in slowly, nice transistion

	the_renderer = graphics_get_renderer();
	do
	{
		sprite_draw(background, 0, vect2d_new(0, 0));

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			back_question();
			if(back == 1)
			{
				done = 1;
			}
			back = 0;
		}

	}while(!done);
}


void back_question()
{
	SDL_Renderer *the_renderer;
	int done = 0;
	Button *yesButton = NULL;
	Button *noButton = NULL;
	Sprite *background = NULL;
	Entity *cam = camera_get();
	Vect2d positionsOffset;

	if(cam)
	{
		positionsOffset = cam->position;
	}
	else
	{
		positionsOffset = vect2d_new(0,0);
	}
	yesButton = button_load_yes_back(vect2d_new(400 + positionsOffset.x, 600 + positionsOffset.y));
	yesButton->click = &yes_back;
	noButton = button_load_no_back(vect2d_new(700 + positionsOffset.x, 600 + positionsOffset.y));
	noButton->click = &no_back;

	background = sprite_load("images/back_screen.png", vect2d_new(1366, 768), 1, 1);
	SDL_SetTextureAlphaMod(background->image, 100); //makes the screen fade in slowly, nice transistion

	the_renderer = graphics_get_renderer();
	do
	{
		sprite_draw(background, 0, positionsOffset);

		button_update(yesButton);
		button_update(noButton);

		button_draw(yesButton);
		button_draw(noButton);

		graphics_next_frame();
		SDL_PumpEvents();

		if(back == 1)
		{
			done = 1;
		}
		if(back == -1)
		{
			done = 1;
			back = 0;
		}
	}while(!done);
	button_free(&yesButton);
	button_free(&noButton);
}

void yes_back()
{
	back = 1;
}

void no_back()
{
	back = -1;
}