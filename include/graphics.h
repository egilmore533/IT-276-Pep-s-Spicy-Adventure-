#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "SDL.h"

/** @file	graphics.h
 *	@brief	Handles everything to do with the Window, renderer, and frame rate
 */

/** 
 *  @brief example macro to return the largest of two values.
 *         can be read as: if a greater than b then a, else b
 */
#define MAX(a,b) (a>b?a:b)

/** @brief example macro to return the smallest of two values.
 *			can be read as: if a less than b then a, else b
 */
#define MIN(a,b) (a<b?a:b)

/** @brief constant to be used to get the size of the window's width */
#define WINDOW_WIDTH	1366

/** @brief constant to be used to get the size of the window's height */
#define WINDOW_HEIGHT	768

/**
 * @brief	initializes the main window and the main renderer.
 * @param [in,out]	windowName	If non-null, name of the window, will be displayed at the top of the window.
 * @param	viewWidth		  	Width of the view.
 * @param	viewHeight		  	Height of the view.
 * @param	renderWidth		  	Width of the render.
 * @param	renderHeight	  	Height of the render.
 * @param	fullscreen		  	flag to use fullscreen or not.
 */
void graphics_initialize(char *windowName, int viewWidth, int viewHeight, int renderWidth, int renderHeight, int fullscreen);

/** @brief closes the window and the renderer at exit */
void graphics_close();

/** @brief	goes to the next frame then holds for a frame delay. */
void graphics_next_frame();

/** @brief	delay's frame rate so the screen and code are synched up properly */
void graphics_frame_delay();

/**
 * @brief	getter for the game's renderer so the rest of the code can use it.
 * @return	a SDL_Renderer* used for all the game's rendering.
 */
SDL_Renderer *graphics_get_renderer();

#endif