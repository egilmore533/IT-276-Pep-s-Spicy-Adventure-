#ifndef __BACKGROUNDS_H__
#define __BACKGROUNDS_H__

#include "sprite.h"

/** @file	backgrounds.h
 *	@brief	Parallax Scrolling system used to contain and handle backgrounds
 */

/** @struct Background structure
 *	@brief the struct for background layers so that they can be drawn infinitely at a unique speed 
 */
typedef struct Background_s
{
	Uint8		alive;													/**< flag to know if the background is currently active */
	Uint8		moving;													/**< flag to disable and enable background movement */
	Uint8		type;													/**< background type, front, middle, back, or flair */

	Sprite		*sprite;												/**< sprite component of the background, used by main and chaser */

	Vect2d		mainPosition;											/**< x and y coordinates of the background's main image */
	Vect2d		chaserPosition;											/**< x and y coordinates of the chaser image */
	int			frameNumber;											/**< the frame number of this background */
	Vect2d		velocity;												/**< the velocity this background will move at, determined in comparison to the camera's velocity */
	Uint32		nextThink;												/**< the next time background will go to the next frame */
	Uint32		thinkRate;												/**< the rate at which this background will cycle its frames */

	void (*draw)	(Sprite *sprite, int frame, Vect2d drawPosition);	/**< draw function for the background, most if not all backgrounds will just be using sprite_draw */
	void (*update)	(struct Background_s *self);						/**< update function for changing the background's data and checking its status */
	void (*free)	(struct Background_s **self);						/**< function to free the background from the list */
}Background;

/** @struct BackgroundPak structure
 *	@brief a collection of backgrounds, used by level structure
 */
typedef struct Background_p
{
	Background *front;							/**< the front most background */
	Background *middle;							/**< the middle background */
	Background *back;							/**< the furthest background */
	Background *flair;							/**< flair for the backgrounds */
	char *name;									/**< name of this backgroundPak */

	void (*free) (struct Background_p *self);	/**< free function that will free all the backgroundPak's data */
}BackgroundPak;

/**
 * @brief basic free function for a background, also removes the pointer to the background so its no longer in the backgroundList
 * @param [in,out] background		double pointer to the background
 */
void background_free(Background **background);

/**
 * @brief	closes the backgroundList, freeing all backgrounds, and resets the backgroundMax to be 0
 */
void background_close_system();

/**
 * @brief initializes the backgroundList so it can be used to load layers of the background
 * @param maxBackground	the maximum number of backgrounds that the list will allow, exceeding this number will cause the program to close
 */
void background_initialize_system(int maxBackground);

/**
 * @brief	returns the first availabe backround in the backgroundList and sets it to be alive
 * @param	moveFactor		the factor at which the background will move in relation to the camera
 * @param	nextThink		the time of the next frame will be set
 * @param	thinkRate		the rate at which the frames will cycle
 * @return	if non-null, the first background that is available in the backgroundList
 */
Background *background_new(float moveFactor, Uint32 nextThink, Uint32 thinkRate);

/**
 * @brief	loads backgrounds from the given config file
 * @param	[in] file		the config file to load backgrounds from
 */
BackgroundPak *background_pak_new(char *file);


/**
 * @brief loops through all backgrounds, and draws their sprites twice once in the main position and once in the chaser position
 */
void background_draw_all();

/**
 * @brief	loops through all backgrounds performing their update functions
 */
void background_update_all();

/**
 * @brief	updates the background's main and chaser positions according to the background's velocity
 *			when the chaser is entirely on the screen it becomes the main and the chaser is moved back past the new main
 * @param	[in,out] background		the background that is being updated
 */
void background_update(Background *background);

#endif