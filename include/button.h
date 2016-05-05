#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "sprite.h"

enum
{
	Normal = 0,
	Hovered,
}ButtonStates;

typedef struct Button_t
{
	Uint8 alive;
	Uint8 state;
	Vect2d position;
	Sprite *buttonSprite;
	Sprite *label;
	void (*click)();
	void (*draw)(struct Button_t *self);
	void (*update)(struct Button_t *self);
}Button;

void button_empty_list();

void button_close_system();

void button_initialize_system(int buttonsMax);

void button_free(Button **button);

Button *button_new(Vect2d position);

void button_draw(Button *button);

void button_update(Button *button);

int mouse_button_intersect(Button *button, Vect2d mousePosition);

void button_update_all();

void button_draw_all();

/////////////////////////////////////////////////////////

Button *button_load_arcade_mode(Vect2d position);

Button *button_load_editor_mode(Vect2d position);

Button *button_load_yes_back(Vect2d position);

Button *button_load_no_back(Vect2d position);

Button *button_load_controls(Vect2d position);

Button *button_load_next(Vect2d position);

Button *button_load_previous(Vect2d position);

#endif