#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "sprite.h"

/** @file	mouse.h
 *	@brief	Mouse that is used by the editor to drop entity actors onto the screen, also handles which entityType the editor is currently on
 */

/**
 * @struct Mouse structure
 * @brief simple tool to track the computer's cursor and track when it clicks, optional sprite that will be instead drawn to the screen instead of the classic mouse BROKEN FOR PLAYER INPUT
 */
typedef struct Mouse_s
{
	Uint8 alive;				/**< the flag to determine if the mouse is alive */
	Vect2d position;			/**< x and y coordinates of the mouse */
	SDL_Rect bounds;			/**< bounds of the mouse (1 by 1) */
	Sprite *sprite;				/**< the mouse's sprite, using template from DJ, Bo made one and never gave it to me */
	int frameNumber;			/**< frameNumner of the mouse */
	Uint8 clicked;				/**< a flag to know if the mouse is clicked or not */
}Mouse;

/** @brief initializes the mouse for use */
void mouse_initialize();

/** @brief updates the mouses position, tells if the mouse has been clicked and calls the mouse_draw, cycles through frames, will place actors on the screen if the editing mode is turned on */
void mouse_update();

/** @brief frees the mouse from memory */
void mouse_free();

/** @brief draws the mouse on the screen at the position it is in */
void mouse_draw();

/** @brief gets the current entity type the player is placing and returns it */
Uint8 currentType_get();

/** @brief returns the mouse so somebody can use it for determining where it is or whether it is clicking */
Mouse *mouse_get();

/** @brief cycles to the next entityType in the list of available ones we will be placing (ENEMY_CLARENCE through POWER_UP_BOMB) */
void mouse_type_next();

/** @brief cycles to the previous entityType in the list of available ones we will be placing (POWER_UP_BOMB through ENEMY_CLARENCE) */
void mouse_type_previous();

/** @brief turns editing mode on */
void mouse_editor_on();

/** @brief turns editing mode off */
void mouse_editor_off();

#endif
