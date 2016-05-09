#ifndef __GAME_H__
#define __GAME_H__

/** @file	game.h
 *	@brief	this is THE file, this is the file that starts everything and runs all the differnet game loops, this is where main is defined, this is where we initialize all the systems for the game
 */


#define MAX_SOUNDS			128			/**< The maximum number of sounds that can be loaded to memory at once (note: sounds can be reused multiple times so you can have as many references to those 128 sounds as you want) */
#define MAX_MUSICS			8			/**< The maximum number of musics that can be loaded to memory at once (note: musics can be reused multiple times so you can have as many references to those 4 sounds as you want) */
#define MAX_SOUNDPAKS		64			/**< The maximum number of soundpaks that can be loaded to memory at once (note: soundpaks can be reused multiple times so you can have as many references to those 32 sounds as you want) */

#define	MAX_SPRITES			1024		/**< The maximum number of sprites the game will allocate memory for (note: sprites can be reused multiple times so you can have as many references to those 1024 sprites as you want) */
#define MAX_PARTICLES		2048		/**< The maximum number of particles the game will allow to be loaded at once */
#define MAX_ENTITIES		1024		/**< The maximum number of entities the game will allocate memory for */
#define MAX_BACKGROUNDS		16			/**< The maximum number of backgrounds the game will allow */
#define MAX_BUTTONS			32			/**< The maximum number of buttons the game will allow to be used at once (buttons on the main menu aren't freed when you enter a game loop so be careful )*/
#define MAX_ACTORS			100			/**< The maximum number of actors the game will allow (essentially the cap for the number of entities the edited level will load)*/


/** 
 * @struct Score structure
 * @brief Used to make the highscore linkedList, my linkedList is left to be only of size 10, and is insert sorted by score
 */
typedef struct Score_t
{
	Uint32 score;				/**<  this number of points that his score has*/
	struct Score_t *next;		/**< the next score in the linkedList */
}Score;

/**
 * @brief initializes all the systems to be used during the game including, logger, graphics, audio, sprite, particles, entity, background, button, and actor
 *			and the highscores linkedLists as well as font that are used in the main menues
 */
void initialize_all_systems();

/**
 * @brief clears out the entityList, backgroundList, and saves the player
 */
void purge_systems();

/**
 * @brief cleans up all the systems, not currently used
 */
void clean_up_all();

/**
 * @brief asks if the player wants to go back, and sets the global back variable properly to do so, uses two buttons and the yes_back and no_back
 */
void back_question();

/**
 * @brief sets the back global variable to be true, which sends the current game loop back to the previous
 */
void yes_back();

/**
 * @brief sets the back global variable to be false, which tells the current game loop to continue
 */
void no_back();

/**
 * @brief shows the player's current score displays it on screen, and waits for the player to press the continue button
 */
void continue_screen();	

/**
 * @brief the main menu which displays the buttons for entering challenge_mode, arcade_mode, control_screen, editor_mode, endless_mode, challenge_highscores_view, and arcade_highscores_view
 */
void main_menu();

/**
 * @brief enters the edited level, updating the challenge highscore list if the player wins, else just game over and show the highscores without entering the players
 */
void challenge_mode();

/**
 * @brief enters the first level, checking for when the player reaches the end of the level, entering the next and updating the player score, on exit or game over update the arcade_highscores with the players last score
 */
void arcade_mode();

/**
 * @brief diplsays the controls of the game using one sprite the size of the screen
 */
void control_screen();

/**
 * @brief has a hud with two buttons that cycle through which type of actor to be placed and a display showing the current type of actor that will be placed when the player clicks the screen, saves the level on exit, and erases the challenge highscores
 */
void editor_mode();

/**
 * @brief enters a level and only exits on player game over, then 
 */
void endless_mode();

/**
 * @brief displays the highscores from the given head
 * @param [in]	head	the head of the list of highscores that are to be shown
 */
void highscores_view(Score *head);	

/**
 * @brief used to call highscores_view passing it the head of the challenge highscores
 */
void challenge_highscores_view();

/**
 * @brief used to call highscores_view passing it the head of the arcade highscores
 */
void arcade_highscores_view();

/**
 * @brief updates the arcade highscores list checking if the given playerScore is greater than any of the entries currently inside and will insert the score at that point in the linked list freeing any scores exceeding the count of ten
 * @param playerScore the score to chekc against the linkedList
 */
void update_arcade_highscores(Uint32 playerScore);

/**
 * @brief updates the challenge highscores list checking if the given playerScore is greater than any of the entries currently inside and will insert the score at that point in the linked list freeing any scores exceeding the count of ten
 * @param playerScore the score to chekc against the linkedList
 */
void update_challenge_highscores(Uint32 playerScore);

/**
 * @brief frees the arcade highscoreList from memory, if write is true also write to the def file in memory setting all scores to zero
 * @param write	if true set all scores in the highscore file to be 0 as well as freeing the linked list
 */
void erase_arcade_highscores(int write);

/**
 * @brief frees the challenge highscore List from memory, if write is true also write to the def file in memory setting all scores to zero
 * @param write	if true set all scores in the highscore file to be 0 as well as freeing the linked list
 */
void erase_challenge_highscores(int write);

/**
 * @brief writes to the given file, its corresponding linkedList's scores saving it for future play throughs
 * @param [in] file		the highscore file to be written to, also selects which head to start at
 */
void write_highscores(char *file);

/**
 * @brief reads the highscores from the given file entering them into the corresponding linkedList 
 * @param [in] file		the highscore file to be written to, also selects which head to start at
 */
void get_highscores(char *file);	

#endif 