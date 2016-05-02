#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "SDL_image.h"
#include "SDL_ttf.h"
#include "vector.h"

/** @file	sprite.h
 *	@brief	Sprite resource management and drawing to the window
 */

typedef struct Sprite_t
{
	char *filename;
	Vect2d frameSize;
	SDL_Texture *image;
	int refCount;
	int fpl;
	int frames;
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

void sprite_empty_list();

void sprite_bloom_effect_draw(Sprite *bloom, int frame, Vect2d position);

void sprite_text_draw(Sprite *text_sprite, Vect2d drawPos);

Sprite* sprite_load_text(TTF_Font *font, char *text, SDL_Color color);

#endif