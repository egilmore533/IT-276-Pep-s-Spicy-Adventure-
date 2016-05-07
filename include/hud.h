#ifndef __HUD_H__
#define __HUD_H__

#include "entity.h"
#include "glib.h"

/** @file	hud.h
 *	@brief	handles the Heads Up Display, how it draws
 */

/**
 * @struct HUD
 * @brief a collection of sprites and labels, which will use the player and camera to determine what to draw, and where to draw it
 */
typedef struct 
{
	Entity	*player;
	Entity	*camera;
	Sprite	*background;

	Sprite	*lives;
	Sprite	*livesLabel;

	Sprite  *bombs;
	Sprite  *bombsLabel;

	Sprite	*pointsLabel;
	Sprite	*pointsMultiplierLabel;
}HUD;

/** @brief initializes the hud, by allocating its memory, loading its sprites and getting the player and camera entities */
void hud_initialize();

/** @brief draws Pep's Spicy Advture HUD, which displays the number of lives and bombs the player has remaining, soon to include points*/
void hud_draw();

/** @brief frees the hud from memory, including all of its sprites and setting its Entity pointers to be NULL (doesn't free the player or camera just sets the hud's pointers to be NULL) */
void hud_free();

/** 
 * @brief returns the hud to be used if needed
 * @return the hud if it exists 
 */
HUD *hud_get();

/** @brief initializes the hud for editing mode, has two buttons loading, and a background */
void hud_editor_initialize();

/** @brief draws the hud background, buttons then entity that is going to be placed, also handles updates for the buttons, uses bombs memeber of hud to store the image of what entity is being drawn */
void hud_editor_draw();

/** @brief frees the editing mode hud, also frees the buttons */
void hud_editor_free();

#endif