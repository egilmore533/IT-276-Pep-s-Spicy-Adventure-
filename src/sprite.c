#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL_image.h"
#include "SDL_ttf.h"
#include "sprite.h"
#include "simple_logger.h"
#include "graphics.h"
#include "camera.h"

static Sprite *spriteList = NULL;
static int spriteNum;
static int spriteMax = 0;

void sprite_free(Sprite **sprite)
{
	Sprite *target;
	if(!sprite)
	{
		return;
	}
	else if(!*sprite)
	{
		return;
	}
	target = *sprite;

	target->refCount--;
	if(target->refCount == 0)
	{
		
		if(target->image != NULL)
		{
			SDL_DestroyTexture(target->image); 
			memset(target,0,sizeof(Sprite));
		}
		spriteNum--;
	}
	*sprite = NULL;
}

void sprite_close_system()
{
	int i;
	if(!spriteList)
	{
		slog("spriteList not initialized");
		return;
	}
	for(i = 0; i < spriteMax; ++i)
	{
		if(spriteList[i].image != 0)
		{
			SDL_DestroyTexture(spriteList[i].image);
		}
	}
	free(spriteList);
	spriteList = NULL;
	spriteNum = 0;
	spriteMax = 0;
}

void sprite_initialize_system(int maxSprite)
{
	int i;
	if(maxSprite == 0)
	{
		slog("Max sprite == 0");
		return;
	}
	spriteList = (Sprite *)malloc(sizeof(Sprite) * maxSprite);
	if(!spriteList)
	{
		slog("spriteList failed to initialize");
		return;
	}
	memset(spriteList, 0, sizeof(Sprite) * maxSprite);
	spriteNum = 0;
	for(i = 0; i < maxSprite; ++i)
	{
		spriteList[i].image = NULL;
	}
	spriteMax = maxSprite;
	atexit(sprite_close_system);
}

Sprite *sprite_load(char *filename, Vect2d frameSize, int fpl, int frames)
{
	int i;
	SDL_Surface *tempSurface;
	SDL_Texture *tempTexture;
	Sprite *sprite = NULL;
	SDL_Renderer *renderer = graphics_get_renderer();

	if(!spriteList)
	{
		slog("spriteList uninitialized");
		return NULL;
	}
	/*first search to see if the requested sprite image is alreday loaded*/
	for(i = 0; i < spriteMax; i++)
	{
		if(spriteList[i].refCount == 0)
		{
			//this makes it so that the next sprite available in the list will be used if no sprite is found to match this one
			if(sprite == NULL)
				sprite = &spriteList[i];
			continue;
		}
		if(strncmp(filename, spriteList[i].filename, 128) ==0)
		{
			spriteList[i].refCount++;
			return &spriteList[i];
		}
	}
	/*makesure we have the room for a new sprite*/
	if(spriteNum + 1 > spriteMax)
	{
		slog("Maximum Sprites Reached.");
		exit(1);
	}

	/*if its not already in memory, then load it.*/
	spriteNum++;
	tempSurface = IMG_Load(filename);

	if(tempSurface == NULL)
	{
		slog("unable to load sprite as a surface");
		exit(0);
	}
	else
	{
		/*sets a transparent color for blitting.*/
		SDL_SetColorKey(tempSurface, SDL_TRUE , SDL_MapRGB(tempSurface->format, 255,255,255));
	
		tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		if(tempTexture == NULL)
		{
			slog("unable to load sprite as a Texture");
			exit(0);
		}
	}
	
	/*then copy the given information to the sprite*/
	sprite->image = tempTexture;	
	sprite->fpl = fpl;
	sprite->frameSize.x = frameSize.x;
	sprite->frameSize.y = frameSize.y;
	sprite->filename = filename;
	sprite->frames = frames;
	sprite->refCount++;
	SDL_FreeSurface(tempSurface);
	return sprite;
}

void sprite_draw(Sprite *sprite, int frame, Vect2d drawPos)
{
	Vect2d positionRelative;
	Entity *cam;
	SDL_Rect source, destination;
	SDL_Renderer *renderer = graphics_get_renderer();
	cam = camera_get();
	if(cam)
	{
		vect2d_subtract(drawPos, cam->position, positionRelative); 
	}
	else
	{
		positionRelative = drawPos;
	}
	if(!sprite)
	{
		slog("sprite doesn't point to anything");
		return;
	}
	source.x = frame % sprite->fpl * sprite->frameSize.x;
	source.y = frame / sprite->fpl * sprite->frameSize.y;
	source.w = sprite->frameSize.x;
	source.h = sprite->frameSize.y;
	
	destination.x = positionRelative.x;
	destination.y = positionRelative.y;
	destination.w = sprite->frameSize.x;
	destination.h = sprite->frameSize.y;
	SDL_RenderCopy(renderer, sprite->image, &source, &destination);
}

void sprite_empty_list()
{
	int i;
	Sprite *sprite = NULL;
	if(!spriteList)
	{
		slog("spriteList not initialized");
		return;
	}
	for(i = 0; i < spriteMax; ++i)
	{
		sprite = &spriteList[i];
		sprite_free(&sprite);
	}
	memset(spriteList, 0, sizeof(Sprite) * spriteMax);
	for(i = 0; i < spriteMax; ++i)
	{
		spriteList[i].image = NULL;
	}
	spriteNum = 0;
}

void sprite_bloom_effect_draw(Sprite *bloom, int frame, Vect2d position)
{
	int i;
	int size_factor, offset_factor_x, offset_factor_y;
	Vect2d positionRelative; 
	Entity *cam;
	SDL_Rect source, destination;
	SDL_Renderer *renderer = graphics_get_renderer();
	cam = camera_get();
	if(cam)
	{
		vect2d_subtract(position, cam->position, positionRelative); 
	}
	else
	{
		positionRelative = position;
	}
	source.x = frame % bloom->fpl * bloom->frameSize.x;
	source.y = frame / bloom->fpl * bloom->frameSize.y;
	source.w = bloom->frameSize.x;
	source.h = bloom->frameSize.y;
	
	for(i = 0; i < 10;i++)
	{
		size_factor = rand() % 25;	
		offset_factor_x = rand() % 10 - 5;
		offset_factor_y = rand() % 10 - 5;
		destination.x = positionRelative.x - size_factor / 2 + offset_factor_x;
		destination.y = positionRelative.y - size_factor / 2 + offset_factor_y;
		destination.w = bloom->frameSize.x + size_factor;
		destination.h = bloom->frameSize.y + size_factor;
		SDL_SetTextureAlphaMod(bloom->image, 40);
		SDL_SetTextureColorMod(bloom->image, 255 - (offset_factor_x + offset_factor_y) * 10, 255 - (offset_factor_x + offset_factor_y) * 10, 255);
			
		SDL_RenderCopy(renderer, bloom->image, &source, &destination);	
	}
	SDL_SetTextureAlphaMod(bloom->image, 255);
	SDL_SetTextureColorMod(bloom->image, 255, 255, 255);
}


Sprite* sprite_load_text(TTF_Font *font, char *text, SDL_Color color)
{
	int i;
	Sprite *sprite = NULL;
	SDL_Surface* textSurface = NULL;
	SDL_Texture* textTexture = NULL;
	SDL_Renderer *renderer = graphics_get_renderer();

	if(!font)
	{
		slog("font doesn't point to anything");
		return NULL;
	}
	if(!text)
	{
		slog("no text to load");
		return NULL;
	}

	if(!spriteList)
	{
		slog("spriteList uninitialized");
		return NULL;
	}
	/*first search to see if the requested sprite image is alreday loaded*/
	for(i = 0; i < spriteMax; i++)
	{
		if(spriteList[i].refCount == 0)
		{
			//this makes it so that the next sprite available in the list will be used if no sprite is found to match this one
			if(sprite == NULL)
				sprite = &spriteList[i];
			continue;
		}
		if(strncmp(text, spriteList[i].filename, 128) ==0)
		{
			spriteList[i].refCount++;
			return &spriteList[i];
		}
	}
	/*makesure we have the room for a new sprite*/
	if(spriteNum + 1 > spriteMax)
	{
		slog("Maximum Sprites Reached.");
		exit(1);
	}

	textSurface = TTF_RenderText_Blended(font, text, color);
	if(!textSurface)
	{
		slog("error loading text as surface");
		return NULL;
	}
	else
	{
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	}
	if(!textTexture)
	{
		slog("error loading text surface to a texture");
		return NULL;
	}

	/*then copy the given information to the sprite*/
	sprite->image = textTexture;
	sprite->filename = text;
	sprite->frameSize.x = textSurface->w;
	sprite->frameSize.y = textSurface->h;
	sprite->refCount++;
	SDL_FreeSurface(textSurface);
	return sprite;
}

void sprite_text_draw(Sprite *text_sprite, Vect2d drawPos)
{
	Vect2d positionRelative; // remove = drawPos after entity and camera are up
	Entity *cam;
	SDL_Rect source, destination;
	SDL_Renderer *renderer = graphics_get_renderer();
	cam = camera_get();
	if(cam)
	{
		vect2d_subtract(drawPos, cam->position, positionRelative); 
	}
	else
	{
		positionRelative = drawPos;
	}
	if(!text_sprite)
	{
		slog("text_sprite doesn't point to anything");
		return;
	}
	source.x = 0;
	source.y = 0;
	source.w = text_sprite->frameSize.x;
	source.h = text_sprite->frameSize.y;
	
	destination.x = positionRelative.x;
	destination.y = positionRelative.y;
	destination.w = text_sprite->frameSize.x;
	destination.h = text_sprite->frameSize.y;
	SDL_RenderCopy(renderer, text_sprite->image, &source, &destination);
	
}
