#include <stdio.h>
#include <math.h>

#include "SDL_ttf.h"
#include "cJSON.h"
#include "simple_logger.h"

#include "files.h"
#include "graphics.h"
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

void back_question();
void continue_screen();									//needs to be tested
void main_menu();

void challenge_mode();
void arcade_mode();
void control_screen();
void editor_mode();

void arcade_highscores_view();								//needs to be written
void challenge_highscores_view();							//needs to be written

void yes_back();
void no_back();

void merge_sort_scores(Uint32 *unorderedScores, Uint32 left, Uint32 right);	//needs to be tested
void erase_highscores(char *file);											//needs to be tested
void write_highscores(char *file, Uint32 *topTenScores);					//needs to be tested
Uint32 *get_highscores(char *file);											//needs to be tested

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
	entity_initialize_system(1024);

	background_initialize_system(8);

	button_initialize_system(20);

	actor_initialize_system(100);

}

void arcade_mode()
{
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;
	Uint8 level_num = 1;
	Level *level = NULL;
	char *level_path = NULL;
	Uint32 playerScore = 0;
	Uint32 *scores;

	SDL_ShowCursor(SDL_ENABLE);
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
			break;
		}

		if(level_end_reached(level))
		{
			playerScore = level->player->points;
			SDL_ShowCursor(SDL_DISABLE);
			continue_screen();
			SDL_ShowCursor(SDL_ENABLE);

			purge_systems();
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
			SDL_ShowCursor(SDL_DISABLE);
			back_question();
			if(back == 1)
			{
				player_saved_load_off();
				purge_systems();
				done = 1;
			}
			back = 0;
			SDL_ShowCursor(SDL_ENABLE);
		}
	}while(!done);
	hud_free();
	SDL_ShowCursor(SDL_DISABLE);

	scores = get_highscores(ARCADE_HIGHSCORES);
	scores[10] = playerScore;
	merge_sort_scores(scores, 0, 10);
	write_highscores(ARCADE_HIGHSCORES, scores);

	arcade_highscores_view();
}

void editor_mode()
{
	const Uint8 *keys = NULL;
	SDL_Renderer *the_renderer;
	int done = 0;
	Entity *camera = NULL;

	camera = camera_load(0);
	camera->think = &camera_editor_think;

	mouse_editor_on();
	hud_editor_initialize();
	the_renderer = graphics_get_renderer();

	do
	{
		SDL_RenderClear(the_renderer);

		camera->think(camera);
		camera->update(camera);

		actor_draw_all();

		hud_editor_draw();

		mouse_update();

		graphics_next_frame();
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);
		if(keys[SDL_SCANCODE_ESCAPE])
		{
			mouse_editor_off();
			back_question();
			if(back == 1)
			{
				done = 1;
			}
			back = 0;
			mouse_editor_on();
		}
	}while(!done);
	level_save();
	mouse_editor_off();
	camera->free(&camera);
	hud_editor_free();
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
	Button *challengeMode = NULL;
	Sprite *sprite = NULL;


	mouse_initialize();

	//init main menu
	//loop until player selects next game mode
	arcadeButton = button_load_arcade_mode(vect2d_new(100, 400));
	arcadeButton->click = &arcade_mode;

	controlButton = button_load_controls(vect2d_new(700, 400));
	controlButton->click = &control_screen;

	editorButton = button_load_editor_mode(vect2d_new(100, 600));
	editorButton->click = &editor_mode;

	challengeMode = button_load_challenge_mode(vect2d_new(700, 600));
	challengeMode->click = &challenge_mode;

	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		button_update_all();
		button_draw_all();

		mouse_update();

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
	mouse_free();
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

void challenge_mode()
{
	int win = 0;
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;
	Uint8 level_num = 1;
	Level *level = NULL;
	char *level_path = NULL;

	SDL_ShowCursor(SDL_ENABLE);
	purge_systems();
	the_renderer = graphics_get_renderer();
	level_path = CHALLENGE_LEVEL;
	level = level_load(level_path);
	hud_initialize();
	done = 0;
	do
	{
		SDL_RenderClear(the_renderer);
	
		if(level->player->state == GAME_OVER_STATE)
		{
			purge_systems();
			level_free(&level);
			done = 1;
			continue;
		}

		if(level_end_reached(level))
		{
			purge_systems();
			level_free(&level);
			win = 1;//victory
			done = 1;
			continue;
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
			SDL_ShowCursor(SDL_DISABLE);
			back_question();
			if(back == 1)
			{
				purge_systems();
				done = 1;
			}
			back = 0;
			SDL_ShowCursor(SDL_ENABLE);
		}
	}while(!done);
	hud_free();
	SDL_ShowCursor(SDL_DISABLE);
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

		mouse_update();

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

void continue_screen()
{
	SDL_Renderer *the_renderer;
	int done = 0;
	Button *continueButton = NULL;
	Sprite *background = NULL;
	Sprite *score = 0;
	char tempScore[30];
	Entity *cam = camera_get();
	Entity *player = player_get();
	Vect2d positionsOffset;
	Vect2d scorePos;

	if(cam)
	{
		positionsOffset = cam->position;
	}
	else
	{
		positionsOffset = vect2d_new(0,0);
	}
	scorePos.x = positionsOffset.x + 600;
	scorePos.y = positionsOffset.y + 100;
	continueButton = button_load_continue(vect2d_new(600 + positionsOffset.x, 600 + positionsOffset.y));
	continueButton->click = &no_back;

	background = sprite_load("images/black_background.png", vect2d_new(1366, 768), 1, 1);
	SDL_SetTextureAlphaMod(background->image, 100); //makes the screen fade in slowly, nice transistion
	sprintf(tempScore, "Score: %d", player->points);
	score = sprite_load_text(font, tempScore, textColor);
	//tempScore = "temp";

	the_renderer = graphics_get_renderer();
	do
	{
		sprite_draw(background, 0, positionsOffset);
		sprite_text_draw(score, scorePos);

		button_update(continueButton);
		button_draw(continueButton);

		mouse_update();

		graphics_next_frame();
		SDL_PumpEvents();

		if(back == -1)
		{
			done = 1;
			back = 0;
		}
	}while(!done);
	button_free(&continueButton);
	sprite_free(&background);
	sprite_free(&score);
}

Uint32 *get_highscores(char *file)
{
	//config file stuff
	cJSON *json, *root;
	FILE *highscore_file;
	long length;
	char *data;

	int i = 0;
	char position[10];
	static Uint32 scores[11];

	highscore_file = fopen(file,"r");
	if(!highscore_file)
	{
		slog("No file found: %s", file);
		return NULL;
	}

	//get the length of the file
	fseek(highscore_file, 0, SEEK_END);
	length = ftell(highscore_file);
	//reset position to start of the file and allocate data to be the length of the file + 1
	fseek(highscore_file, 0, SEEK_SET);
	data = (char*) malloc(length + 1);
	//store the contents of the file in data, and close the file
	fread(data, 1, length, highscore_file);
	fclose(highscore_file);

	json = cJSON_Parse(data);
	root = cJSON_GetObjectItem(json,"scores");
	if(!root)
	{
		slog("error parseing the file, file not a scores file");
		return;
	}
	for(i = 0; i < 10; i++)
	{
		sprintf(position, "%d", i + 1);
		scores[i] = cJSON_GetObjectItem(root, position)->valueint;
	}

	return scores;
}

void write_highscores(char *file, Uint32 *topTenScores)
{
	FILE *highscore_file;
	cJSON *json, *root;
	char *data;
	char *position[10];
	int i;

	json = cJSON_CreateObject();
	cJSON_AddItemToObject(json, "scores", root=cJSON_CreateObject());
	for(i = 0; i < 10; i++)
	{
		sprintf(position, "%d", i + 1);
		cJSON_AddNumberToObject(root, position, topTenScores[i]);
	}

	data = cJSON_Print(json);
	cJSON_Delete(json);

	highscore_file = fopen(file, "w");

	if (file == NULL) {
	  fprintf(stderr, "Can't open output file %s!\n",
			  file);
	  return;
	}

	fprintf(highscore_file, data);

	fclose(highscore_file);

}

void erase_highscores(char *file)
{
	FILE *highscore_file;
	cJSON *json, *root;
	char *data;
	int i;
	char position[10];

	json = cJSON_CreateObject();
	cJSON_AddItemToObject(json, "scores", root=cJSON_CreateObject());
	for(i = 0; i < 10; i++)
	{
		sprintf(position, "%d", i + 1);
		cJSON_AddNumberToObject(root, position, 0);
	}

	data = cJSON_Print(json);
	cJSON_Delete(json);

	highscore_file = fopen(file, "w");

	if (file == NULL) {
	  fprintf(stderr, "Can't open output file %s!\n",
			  file);
	  return;
	}

	fprintf(highscore_file, data);

	fclose(highscore_file);
}

//uses merge sort to sort a list of 11 integers, used in arranging highscores
void merge_sort_scores(Uint32 *unorderedScores, Uint32 left, Uint32 right)
{
	static Uint32 *workspace;
	Uint32 i;
	Uint32 length;
	Uint32 midpointDistance;
	Uint32 rPosition, lPosition;

	if(!workspace)
	{
		workspace = (Uint32 *)malloc(11 * sizeof(Uint32));
	}
	else if(workspace)
	{
		//stops the recursive calls when comparing same position
		if(right == left + 1)
		{
			return;
		}
		//recursively call left and right directions to split the input down (Binary Tree)
		else
		{
			i = 0;
			length = right - left;
			midpointDistance = length/2;

			//positions in the left and right subarray
			lPosition = left;
			rPosition = left + midpointDistance;

			//sort my subarrays
			merge_sort_scores(unorderedScores, left, left + midpointDistance);
			merge_sort_scores(unorderedScores, left + midpointDistance, right);

			//merge together the arrays together using worksace for temporary storage
			for(i = 0; i < length; i++)
			{
				if(lPosition < left + midpointDistance && (rPosition == right || MAX(unorderedScores[lPosition], unorderedScores[rPosition]) == unorderedScores[lPosition]))
				{
					workspace[i] = unorderedScores[lPosition];
					lPosition++;
				}
				else
				{
					workspace[i] = unorderedScores[rPosition];
					rPosition++;
				}
			}
			//copy the workspace sorted array back into input
			for(i = left; i < right; i++)
			{
				unorderedScores[i] = workspace[i - left];
			}
		}
	}
}


void arcade_highscores_view()
{
	SDL_Renderer *the_renderer;
	int done = 0;
	const Uint8 *keys;
	int i;
	Sprite *scoreSprite1 = NULL;
	Sprite *scoreSprite2 = NULL;
	Sprite *scoreSprite3 = NULL;
	Sprite *scoreSprite4 = NULL;
	Sprite *scoreSprite5 = NULL;
	Sprite *scoreSprite6 = NULL;
	Sprite *scoreSprite7 = NULL;
	Sprite *scoreSprite8 = NULL;
	Sprite *scoreSprite9 = NULL;
	Sprite *scoreSprite10 = NULL;
	Uint32 *scores = get_highscores(ARCADE_HIGHSCORES);
	char scoreTemp[255];
	Vect2d drawPos;

	for(i = 0; i < 10; i++)
	{
		sprintf(scoreTemp, "%d: %d", i + 1, scores[i]);
		switch(i)
		{
		case 0:
			scoreSprite1 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 1:
			scoreSprite2 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 2:
			scoreSprite3 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 3:
			scoreSprite4 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 4:
			scoreSprite5 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 5:
			scoreSprite6 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 6:
			scoreSprite7 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 7:
			scoreSprite8 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 8:
			scoreSprite9 = sprite_load_text(font, scoreTemp,textColor);
			break;
		case 9:
			scoreSprite10 = sprite_load_text(font, scoreTemp,textColor);
			break;
		}
		
	}
	

	
	the_renderer = graphics_get_renderer();
	do
	{
		SDL_RenderClear(the_renderer);

		vect2d_set(drawPos, 500, 100);
		for(i = 0; i < 10; i++)
		{
			drawPos.y += 50;
			switch(i)
			{
			case 0:
				sprite_text_draw(scoreSprite1, drawPos);
				break;
			case 1:
				sprite_text_draw(scoreSprite2, drawPos);
				break;
			case 2:
				sprite_text_draw(scoreSprite3, drawPos);
				break;
			case 3:
				sprite_text_draw(scoreSprite4, drawPos);
				break;
			case 4:
				sprite_text_draw(scoreSprite5, drawPos);
				break;
			case 5:
				sprite_text_draw(scoreSprite6, drawPos);
				break;
			case 6:
				sprite_text_draw(scoreSprite7, drawPos);
				break;
			case 7:
				sprite_text_draw(scoreSprite8, drawPos);
				break;
			case 8:
				sprite_text_draw(scoreSprite9, drawPos);
				break;
			case 9:
				sprite_text_draw(scoreSprite10, drawPos);
				break;
			}
		}		

		mouse_update();

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
