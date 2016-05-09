#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SDL_ttf.h"

#include "cJSON.h"
#include "simple_logger.h"

#include "game.h"
#include "actor.h"
#include "background.h"
#include "button.h"
#include "camera.h"
#include "files.h"
#include "graphics.h"
#include "hud.h"
#include "level.h"
#include "mouse.h"
#include "particle.h"
#include "player.h"
#include "sprite.h"


/* menu fonts data */
TTF_Font		*font;
SDL_Color		textColor = {255, 210 ,4};

/* highscore linked lists heads */
Score			*headArcade;
Score			*headChallenge;

/* menu music data */
Music			*titleTheme;

/* back flag to check if the player wants to move back through the menues */
int				back = 0;

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
	titleTheme = audio_load_music("sound/title_theme.ogg", -1);
	audio_play_music(titleTheme);

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

	audio_music_free(&titleTheme);

	write_highscores(ARCADE_HIGHSCORES);
	write_highscores(CHALLENGE_HIGHSCORES);
	
	erase_challenge_highscores(0);
	erase_arcade_highscores(0);

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
	font = TTF_OpenFont(PEP_FONT, 28);

	init_logger(LOG_FILE); //init simple logger from DJ's source code
	
	graphics_initialize("Pep's Spicy Adventure", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	audio_initialize(MAX_SOUNDS, MAX_MUSICS, MAX_SOUNDPAKS);

	sprite_initialize_system(MAX_SPRITES);
	particle_initialize_system(MAX_PARTICLES);
	entity_initialize_system(MAX_ENTITIES);

	background_initialize_system(MAX_BACKGROUNDS);

	button_initialize_system(MAX_BUTTONS);

	actor_initialize_system(MAX_ACTORS);

	get_highscores(ARCADE_HIGHSCORES);
	get_highscores(CHALLENGE_HIGHSCORES);
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

	update_arcade_highscores(playerScore);
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

	erase_challenge_highscores(1);
	get_highscores(CHALLENGE_HIGHSCORES);
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
	Button *arcadeHighscores = NULL;
	Button *challengeHighscores = NULL;
	Button *endlessMode = NULL;
	Sprite *sprite = NULL;


	mouse_initialize();

	//init main menu
	//loop until player selects next game mode, then enter that loop, or until player exits
	arcadeButton = button_load_big(vect2d_new(100, 300), "Arcade Mode");
	arcadeButton->click = &arcade_mode;

	controlButton = button_load_big(vect2d_new(700, 300), "Controls");
	controlButton->click = &control_screen;

	editorButton = button_load_big(vect2d_new(100, 450), "Editor Mode");
	editorButton->click = &editor_mode;

	challengeMode = button_load_big(vect2d_new(700, 450), "Challenge Mode");
	challengeMode->click = &challenge_mode;

	arcadeHighscores = button_load_big(vect2d_new(100, 150), "Arcade Highscores");
	arcadeHighscores->click = &arcade_highscores_view;

	challengeHighscores = button_load_big(vect2d_new(700, 150), "Challenge Highscores");
	challengeHighscores->click = &challenge_highscores_view;

	endlessMode = button_load_big(vect2d_new(100, 600), "Endless Mode");
	endlessMode->click = &endless_mode;

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
	//audio_empty_list();
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
	Uint32 playerScore = 0;

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
			playerScore = level->player->points;
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

	update_challenge_highscores(playerScore);
	challenge_highscores_view();
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
	yesButton = button_load_small(vect2d_new(400 + positionsOffset.x, 600 + positionsOffset.y), "Yes");
	yesButton->click = &yes_back;
	noButton = button_load_small(vect2d_new(700 + positionsOffset.x, 600 + positionsOffset.y), "No");
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
	continueButton = button_load_big(vect2d_new(600 + positionsOffset.x, 600 + positionsOffset.y), "Continue");
	continueButton->click = &no_back;

	background = sprite_load("images/black_background.png", vect2d_new(1366, 768), 1, 1);
	SDL_SetTextureAlphaMod(background->image, 100); //makes the screen fade in slowly, nice transistion
	sprintf(tempScore, "Score: %u", player->points);
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

void get_highscores(char *file)
{
	//config file stuff
	cJSON *json, *root;
	FILE *highscore_file;
	long length;
	char *data;

	int i = 1;
	Score *nextScore;
	char position[10];

	highscore_file = fopen(file,"r");
	if(!highscore_file)
	{
		slog("No file found: %s", file);
		return;
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

	if(file == ARCADE_HIGHSCORES)
	{
		if(!headArcade)
		{
			headArcade = (Score *)malloc(sizeof(Score));
		}
		nextScore = headArcade;
	}
	else
	{
		if(!headChallenge)
		{
			headChallenge = (Score *)malloc(sizeof(Score));
		}
		nextScore = headChallenge;
	}
	while(1)
	{
		sprintf(position, "%d", i++);
		nextScore->score = cJSON_GetObjectItem(root, position)->valueint;
		if(i > 10)
		{
			break;
		}
		else
		{
			nextScore->next = (Score *)malloc(sizeof(Score));
			nextScore = nextScore->next;
			nextScore->next = NULL;
		}
		
	}
}

void update_arcade_highscores(Uint32 playerScore)
{
	Score *insertScore; 
	Score *nextScore;
	Score *prev;
	Score *n;
	int count = 0;
	if(playerScore == 0)
	{
		return;
	}
	if(headArcade->score < playerScore)
	{
		insertScore = (Score *)malloc(sizeof(Score));
		nextScore = headArcade;
		headArcade = insertScore;
		headArcade->next = nextScore;
		headArcade->score = playerScore;
		playerScore = 0;
	}
	for(n = headArcade; n != NULL; n = nextScore)
	{
		nextScore = n->next;
		count++;
		if(count > 10)
		{
			//prev->next = NULL;
			free(n);
		}
		else if(nextScore)
		{
			if(n->score < playerScore)
			{
				insertScore = (Score *)malloc(sizeof(Score));
				insertScore->score = playerScore;
				insertScore->next = nextScore;
				prev->next = insertScore;
				insertScore->next = n;
				playerScore = 0;
			}
		}
		prev = n;
	}
}

void update_challenge_highscores(Uint32 playerScore)
{
	Score *insertScore; 
	Score *nextScore;
	Score *prev;
	Score *n;
	int count = 0;
	if(headChallenge->score < playerScore)
	{
		insertScore = (Score *)malloc(sizeof(Score));
		nextScore = headChallenge;
		headChallenge = insertScore;
		headChallenge->next = nextScore;
		headChallenge->score = playerScore;
		playerScore = 0;
	}
	for(n = headChallenge; n != NULL; n = nextScore)
	{
		nextScore = n->next;
		count++;
		if(count > 10)
		{
			prev->next = NULL;
			free(n);
		}
		else if(nextScore)
		{
			if(n->score < playerScore)
			{
				insertScore = (Score *)malloc(sizeof(Score));
				insertScore->score = playerScore;
				insertScore->next = nextScore;
				prev->next = insertScore;
				insertScore->next = n;
				playerScore = 0;
			}
		}
		prev = n;
	}
}

void write_highscores(char *file)
{
	FILE *highscore_file;
	cJSON *json, *root;
	char *data;
	Score *nextScore;
	char position[11];
	int i = 0;

	if(file == ARCADE_HIGHSCORES)
	{
		if(headArcade == NULL)
		{
			slog("cannot save no, arcade data");
			return;
		}
		nextScore = headArcade;
	}
	else
	{
		if(!headChallenge)
		{
			slog("cannot save no, challenge data");
			return;
		}
		nextScore = headChallenge;
	}

	json = cJSON_CreateObject();
	cJSON_AddItemToObject(json, "scores", root=cJSON_CreateObject());
	while(nextScore != NULL)
	{
		i++;
		if(i > 10)
		{
			break;
		}
		sprintf(position, "%d", i);
		cJSON_AddNumberToObject(root, position, nextScore->score);
		if(nextScore->next == NULL)
		{
			break;
		}
		else
		{
			nextScore = nextScore->next;
		}
	}

	data = cJSON_Print(json);
	cJSON_Delete(json);
	highscore_file = fopen(file, "w");
	if (file == NULL) 
	{
		slog("Can't open output file %s!\n", file);
	}

	fprintf(highscore_file, data);
	fclose(highscore_file);
}

void erase_challenge_highscores(int write)
{
	//config file stuff
	cJSON *json, *root;
	FILE *highscore_file;
	long length;
	char *data;
	char position[11];
	int i = 1;

	Score *temp;
	while( headChallenge != NULL)
	{
		temp = headChallenge;
		headChallenge = headChallenge->next;
		free(temp);
	}

	if(write)
	{
		json = cJSON_CreateObject();
		cJSON_AddItemToObject(json, "scores", root=cJSON_CreateObject());
		for(i = 1; i <= 10; i++)
		{
			sprintf(position, "%d", i);
			cJSON_AddNumberToObject(root, position, 0);
		}

		data = cJSON_Print(json);
		cJSON_Delete(json);

		highscore_file = fopen(CHALLENGE_HIGHSCORES, "w");

		if (highscore_file == NULL) 
		{
		  slog("Can't open output file %s!\n",
				  highscore_file);
		  return;
		}

		fprintf(highscore_file, data);

		fclose(highscore_file);
	}
}

void erase_arcade_highscores(int write)
{
	//config file stuff
	cJSON *json, *root;
	FILE *highscore_file;
	long length;
	char *data;
	char position[11];
	int i = 1;

	Score *temp;
	while( headArcade != NULL)
	{
		temp = headArcade;
		headArcade = headArcade->next;
		free(temp);
	}

	if(write)
	{
		json = cJSON_CreateObject();
		cJSON_AddItemToObject(json, "scores", root=cJSON_CreateObject());
		for(i = 1; i <= 10; i++)
		{
			sprintf(position, "%d", i);
			cJSON_AddNumberToObject(root, position, 0);
		}

		data = cJSON_Print(json);
		cJSON_Delete(json);

		highscore_file = fopen(ARCADE_HIGHSCORES, "w");

		if (highscore_file == NULL) 
		{
		  slog("Can't open output file %s!\n",
				  highscore_file);
		  return;
		}

		fprintf(highscore_file, data);

		fclose(highscore_file);
	}
}

void arcade_highscores_view()
{
	highscores_view(headArcade);
}

void challenge_highscores_view()
{
	highscores_view(headChallenge);
}

void highscores_view(Score *head)
{
	SDL_Renderer *the_renderer;
	int done = 0;
	const Uint8 *keys;
	int i = 0;
	Score *n;
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
	
	char scoreTemp[255];
	Vect2d drawPos;

	audio_play_music(titleTheme);

	for(n = head; n != NULL; n = n->next)
	{
		i++;
		if(i > 10)
		{
			break;
		}
		sprintf(scoreTemp, "%d: %u", i, n->score);
		switch(i)
		{
		case 1:
			scoreSprite1 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 2:
			scoreSprite2 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 3:
			scoreSprite3 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 4:
			scoreSprite4 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 5:
			scoreSprite5 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 6:
			scoreSprite6 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 7:
			scoreSprite7 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 8:
			scoreSprite8 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 9:
			scoreSprite9 = sprite_load_text(font, scoreTemp, textColor);
			break;
		case 10:
			scoreSprite10 = sprite_load_text(font, scoreTemp, textColor);
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
	sprite_free(&scoreSprite1);
	sprite_free(&scoreSprite2);
	sprite_free(&scoreSprite3);
	sprite_free(&scoreSprite4);
	sprite_free(&scoreSprite5);
	sprite_free(&scoreSprite6);
	sprite_free(&scoreSprite7);
	sprite_free(&scoreSprite8);
	sprite_free(&scoreSprite9);
	sprite_free(&scoreSprite10);
}

void endless_mode()
{
	int done;
	const Uint8 *keys;
	SDL_Renderer *the_renderer;
	Uint8 level_num = 1;
	Level *level = NULL;
	char *level_path = NULL;
	Uint32 playerScore = 0;

	float nextLoadPosition = 0;
	float loadPositionRate = 2000;
	int i = 0;
	int spawnNum = 3;
	Entity *entity = NULL;
	Uint32 id = 0;
	int type;
	Vect2d position;

	SDL_ShowCursor(SDL_ENABLE);
	purge_systems();
	the_renderer = graphics_get_renderer();
	level = level_load(ENDLESS_LEVEL);

	hud_initialize();
	player_saved_load_on();
	done = 0;
	while(!done)
	{
		SDL_RenderClear(the_renderer);
	
		if(level->player->state == GAME_OVER_STATE)
		{
			player_saved_load_off();
			purge_systems();
			level_free(&level);
			break;
		}

		if(level->cam->position.x > nextLoadPosition)
		{
			for(i = 0; i < spawnNum; i++)
			{
				if( (rand() % 3) < 1)
				{
					type = 2 + (rand() % 11);
				}
				else
				{
					type = 2 + (rand() % 5);
				}
				entity = level_entity_load(type, id++);
				position.x = level->cam->position.x + (WINDOW_WIDTH * (rand() % 100) * 0.01) + WINDOW_WIDTH;
				position.y = level->cam->position.y + (HUD_HEIGHT * (rand() % 100) * 0.01);
				if(type == ENEMY_CLARENCE)
				{
					position.y = 10;
				}
				entity->position = position;
			}
			loadPositionRate -= rand() % 10;
			nextLoadPosition += loadPositionRate;
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
	}
	hud_free();
	SDL_ShowCursor(SDL_DISABLE);

	audio_play_music(titleTheme);
}