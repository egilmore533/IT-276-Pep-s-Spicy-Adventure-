#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "SDL_image.h"
#include "SDL_ttf.h"
#include "vector.h"

/** @file	sprite.h
 *	@brief	Sprite resource management and drawing to the window
 */

/** 
 * @struct	Sprite structure
 * @brief	an image loaded to memory, with a set frame size and frames per line that is used to draw onto the screen a specific sprite on the image
 */
typedef struct Sprite_t
{
	char *filename;				/**< filepath to the image, used to id the sprites so only one is loaded into the memory and will be rerefrenced on further load calls */
	Vect2d frameSize;			/**< width and height of a single sprite from the image */
	SDL_Texture *image;			/**< entire texture that was loaded to memory */
	int refCount;				/**< number of times the specific sprite has been referenced */
	int fpl;					/**< number of frames per line on the sprite sheet */
	int frames;					/**< numebr of total frames in the sprite sheet, not required, but useful for updating frame numbers */
}Sprite;

/**
 * @brief	removes one reference from spriteList, if the
 * 			refCount is 0 frees the sprite from spriteList and frees the sprite pointer.
 * @param [in,out]	sprite	double pointer to the sprite.
 */
void sprite_free(Sprite **free);

/** @brief	closes the sprite system by freeing the entire sprite list and setting the number of sprites in the list to 0*/
void sprite_close_system();

/**
 *  @brief opens the sprite system by allocating and memsetting the spriteList to have room for the provided number of sprites 
 *  @param	maxSprites	the maximum number of different sprites the spriteList will be able to support
 */
void sprite_initialize_system(int maxSprites);

/** 
 * @brief loads a sprite into the spriteList using the given info
 * @param	[in] filename	the filepath for the image
 * @param	frameSize		2d vector defining how large a frame of the image will be
 * @param	fpl				the frames per line on the image
 * @param	frames			the total number of frames that the image has, used to know when the sprite has gone through the animation
 * @return A pointer to the sprite with the info provided
 */
Sprite *sprite_load(char *filename, Vect2d frameSize, int fpl, int frames);

/**
 * @brief draws the sprite frame to the screen at the position relative to the camera
 * @param	[in] sprite		the image reference to be drawn from
 * @param	frame			the frame of  the image to draw
 * @param	drawPos			2D vector of where the sprite should be drawn in the game world
 */
void sprite_draw(Sprite *sprite, int frame, Vect2d drawPos);

/** @brief	completely frees all the sprites in the list, but leaves the actual spriteList intact */
void sprite_empty_list();

/**
 * @brief draws a sprite to the screen with added randomly generated parameters to make the image look like it is pulsing, changing the alpha, drawPosition, and size of the image randomly
 * @param	[in] sprite		the image reference to be drawn from
 * @param	frame			the frame of  the image to draw
 * @param	drawPos			2D vector of where the sprite should be drawn in the game world
 */
void sprite_bloom_effect_draw(Sprite *bloom, int frame, Vect2d position);

/**
 * @brief draws a text sprite to the screen, since text sprites only have one frame 
 *		  this draw function assumes the frame number is zero and doesn't do any math to determine which part of the image to draw to the screen
 * @param	[in] text_sprite		the image reference to be drawn from
 * @param	drawPos					2D vector of where the sprite should be drawn in the game world
 */
void sprite_text_draw(Sprite *text_sprite, Vect2d drawPos);

/** 
 * @brief loads a text sprite into the spriteList using the given info, 
 *		  since the image is only one frame will determine the frameSize to be exactly the image size
 * @param	[in] font		a loaded TTF_Font that will be used to create the text sprite
 * @param	[in]text		what the text sprite will read as
 * @param	color			SDL_Color code of what the text's color will be (RGB)
 * @return A pointer to the sprite loaded into tmemory using the provided info
 */
Sprite* sprite_load_text(TTF_Font *font, char *text, SDL_Color color);

#endif