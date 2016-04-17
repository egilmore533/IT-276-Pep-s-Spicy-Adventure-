#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "SDL_image.h"
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

void sprite_initialize_system(int maxSprites);

Sprite *sprite_load(char *filename, Vect2d frameSize, int fpl, int frames);

void sprite_draw(Sprite *sprite, int frame, Vect2d drawPos);

#endif