#include <math.h>
#include <stdio.h>
#include "graphics.h"
#include "sprite.h"
#include "simple_logger.h"

static SDL_Window   *   g_graphics_main_window = NULL;
static SDL_Renderer *   g_graphics_renderer = NULL;

/*timing*/
static Uint32 g_graphics_frame_delay = 30;
static Uint32 g_graphics_now = 0;
static Uint32 g_graphics_then = 0;
static Uint8 g_graphics_print_fps = 1;
static float g_graphics_fps = 0; 

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
    g_graphics_main_window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, renderWidth, renderHeight, flags);
    if (!g_graphics_main_window)
    {
        slog("failed to create main window: %s",SDL_GetError());
        graphics_close();
        return;
    }
    
    g_graphics_renderer = SDL_CreateRenderer(g_graphics_main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (!g_graphics_renderer)
    {
        slog("failed to create renderer: %s",SDL_GetError());
        graphics_close();
        return;
    }
    
    SDL_RenderClear(g_graphics_renderer);
    SDL_RenderPresent(g_graphics_renderer);
	//sets a hint with normal priority, this specifically sets the scaling quality to be linear, this is supported by OpenGL and Direct3D
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    //sets a device independent resolution for rendering
	SDL_RenderSetLogicalSize(g_graphics_renderer, renderWidth, renderHeight);
    
    atexit(graphics_close);
    slog("graphics initialized\n");
}

void graphics_frame_delay()
{
	Uint32 diff;
	g_graphics_then = g_graphics_now;
    g_graphics_now = SDL_GetTicks();
    diff = (g_graphics_now - g_graphics_then);
    if (diff < g_graphics_frame_delay)
    {
        SDL_Delay(g_graphics_frame_delay - diff);
    }
    g_graphics_fps = 1000.0/MAX(SDL_GetTicks() - g_graphics_then,0.001);
    if (g_graphics_print_fps)
    {
        printf("FPS: %f\n",g_graphics_fps); //printf so the log isn't filled with frame rate
    }
}

void graphics_next_frame()
{
  SDL_RenderPresent(g_graphics_renderer);
  graphics_frame_delay(); 
}

void graphics_close()
{
    if (g_graphics_renderer)
    {
        SDL_DestroyRenderer(g_graphics_renderer);
    }
    if (g_graphics_main_window)
    {
        SDL_DestroyWindow(g_graphics_main_window);
    }
    g_graphics_main_window = NULL;
    g_graphics_renderer = NULL;
}

SDL_Renderer *graphics_get_renderer()
{
	return g_graphics_renderer;
}

