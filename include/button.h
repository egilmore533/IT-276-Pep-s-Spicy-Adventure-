#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "sprite.h"
#include "mouse.h"

/** @file	button.h
 *	@brief	button class that will allocate space for all buttons in the game, cannot update all at the same time and free all whenever you please because this 
 *			will cause the menu buttons to appear during gameplay and disappear after closing and going back to the menu, buttonList is probably no the best way to go 
 *			about this, just handling them in their own callbacks would probably be the best bet
 */

/**
 * @enum ButtonStates
 * @brief defines the slots inside the player's inventory
 */
enum
{
	NORMAL = 0,		/**< cursor is not over button */
	HOVERED,		/**< cursor is hovering over button */
}ButtonStates;

/** 
 * @struct	Button structure
 * @brief	a simple sprite that checks the mouse for its position and whether it has been clicked and will act accordingly
 */
typedef struct Button_t
{
	Uint8 alive;								/**< whether or not this button is alive */
	Uint8 state;								/**< what state this button is in (HOVERED or NORMAL) */
	Vect2d position;							/**< the location in the game world of the button, will usually update based on where the camera is */
	SDL_Rect bounds;							/**< bounding box of the button, used to compare between the mouse and itself */
	Sprite *buttonSprite;						/**< the backdrop of the button, twp frames to switch between to represent HOVERED, and NORMAL states */
	Sprite *label;								/**< the text label to be drawn on top of the buttons backdrop */
	void (*click)();							/**< function that will occur on button click */
	void (*draw)(struct Button_t *self);		/**< draw function for the button */
	void (*update)(struct Button_t *self);		/**< updates the button to check the mouse for clicks and position */
}Button;

/** @brief	completely frees all the buttons in the list, but leaves the actual buttonList intact */
void button_empty_list();

/** @brief	completely frees all the buttons in the list, freeing the list at the end and setting the numButtons to 0 */
void button_close_system();

/**
 *  @brief opens the button system by allocating and memsetting the buttonList to have room for the provided number of buttons 
 *  @param	buttonsMax	the maximum number of different buttons the buttonList will be able to support
 */
void button_initialize_system(int buttonsMax);

/**
 * @brief	removes the button from the buttonList, freeing its sprites, and the pointer to the button
 * @param [in,out]	button	double pointer to the button.
 */
void button_free(Button **button);

/** 
 * @brief loads a button into the buttonList at the given position
 * @param	position		2d vector defining where the button is in the game world
 * @return A pointer to the button at the position given
 */
Button *button_new(Vect2d position);

/** 
 * @brief draws the button at the given position, draws the buttonSprite backdrop first, then calculating the button's label position to be the center of the button and drawing that
 * @param [in]	button		the button to be drawn at its location
 */
void button_draw(Button *button);

/** 
 * @brief checks for the mouse's position and changes state based on that, also checks the button for input and will run its click function if so
 * @param [in]	button		the button to be checked
 */
void button_update(Button *button);

int mouse_button_intersect(Button *button, Mouse *mouse);

void button_update_all();

void button_draw_all();

////////////////////////SPECIF LOAD FUNCTIONS/////////////////////////////////

Button *button_load_arcade_mode(Vect2d position);

Button *button_load_editor_mode(Vect2d position);

Button *button_load_yes_back(Vect2d position);

Button *button_load_no_back(Vect2d position);

Button *button_load_controls(Vect2d position);

Button *button_load_next(Vect2d position);

Button *button_load_previous(Vect2d position);

Button *button_load_challenge_mode(Vect2d position);

Button *button_load_continue(Vect2d position);

#endif