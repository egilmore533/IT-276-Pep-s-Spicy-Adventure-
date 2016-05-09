#ifndef __GAME_H__
#define __GAME_H__



void initialize_score_list();

typedef struct Score_t
{
	Uint32 score;
	struct Score_t *next;
}Score;

void initialize_all_systems();
void purge_systems();
void clean_up_all();
void initialize_next_level(Uint8 level_number);

void back_question();
void continue_screen();									
void main_menu();

void challenge_mode();
void arcade_mode();
void control_screen();
void editor_mode();
void endless_mode();

void highscores_view(Score *head);							
void challenge_highscores_view();
void arcade_highscores_view();

void yes_back();
void no_back();

void update_arcade_highscores(Uint32 playerScore);
void update_challenge_highscores(Uint32 playerScore);

void erase_arcade_highscores(int write);
void erase_challenge_highscores(int write);

void erase_highscores(char *file);	
void write_highscores(char *file);
void get_highscores(char *file);	

#endif 