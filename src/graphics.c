#include <math.h>
#include <stdio.h>

#include "simple_logger.h"

#include "graphics.h"
#include "camera.h"
#include "sprite.h"


/* rendering pipeline data */
static SDL_Window			*graphicsMainWindow = NULL;
static SDL_Renderer			*graphicsRenderer = NULL;

/* timing */
static Uint32				graphicsFrameDelay = 30;
static Uint32				graphicsNow = 0;
static Uint32				graphicsThen = 0;
static Uint8				graphicsPrintFPS = 1;
static float				graphicsFPS = 0; 

void graphics_initialize(char *windowName, int viewWidth, int viewHeight, int renderWidth, int renderHeight, int fullscreen)
{
    Uint32 flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        slog("Unable to initilaize SDL system: %s",SDL_GetError());
        return;
    }
    atexit(SDL_Quit);
    if (fullscreen)
    {
        if (renderWidth == 0)
        {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        else
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
    }
    graphicsMainWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, renderWidth, renderHeight, flags);
    if (!graphicsMainWindow)
    {
        slog("failed to create main window: %s",SDL_GetError());
        graphics_close();
        return;
    }
    
    graphicsRenderer = SDL_CreateRenderer(graphicsMainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!graphicsRenderer)
    {
        slog("failed to create renderer: %s",SDL_GetError());
        graphics_close();
        return;
    }
    
    SDL_RenderClear(graphicsRenderer);
    SDL_RenderPresent(graphicsRenderer);
	//sets a hint with normal priority, this specifically sets the scaling quality to be linear, this is supported by OpenGL and Direct3D
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    //sets a device independent resolution for rendering
	SDL_RenderSetLogicalSize(graphicsRenderer, renderWidth, renderHeight);
    
    atexit(graphics_close);
    slog("graphics initialized\n");
}

void graphics_frame_delay()
{
	Uint32 diff;
	graphicsThen = graphicsNow;
    graphicsNow = get_time();
    diff = (graphicsNow - graphicsThen);
    if (diff < graphicsFrameDelay)
    {
        SDL_Delay(graphicsFrameDelay - diff);
    }
    graphicsFPS = 1000.0/MAX(SDL_GetTicks() - graphicsThen,0.001);
    if (graphicsPrintFPS)
    {
        printf("FPS: %f\n",graphicsFPS); //printf so the log isn't filled with frame rate
    }
}

void graphics_next_frame()
{
  SDL_RenderPresent(graphicsRenderer);
  graphics_frame_delay(); 
}

void graphics_close()
{
    if (graphicsRenderer)
    {
        SDL_DestroyRenderer(graphicsRenderer);
    }
    if (graphicsMainWindow)
    {
        SDL_DestroyWindow(graphicsMainWindow);
    }
    graphicsMainWindow = NULL;
    graphicsRenderer = NULL;
}

SDL_Renderer *graphics_get_renderer()
{
	return graphicsRenderer;
}

