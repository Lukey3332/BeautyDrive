#include "SDL.h"
#include "sys.h"
#include "game.h"

#define BASEWIDTH	640
#define BASEHEIGHT	480

static SDL_Surface * screen;

void Vid_Init ()
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO))
		Sys_Error("Vid: SDL_InitSubSysten failed!");
	
	if (!(screen = SDL_SetVideoMode(BASEWIDTH, BASEHEIGHT, 24, SDL_SWSURFACE)))
		Sys_Error("Vid: Couldn't set video mode: %s", SDL_GetError());
		
	SDL_WM_SetCaption(GAME_NAME, GAME_NAME);
	SDL_ShowCursor(0);
}

void Vid_Update (void * buffer, uint width, uint height)
{
	SDL_LockSurface(screen);
	memcpy( screen->pixels, buffer, screen->format->BytesPerPixel*width*height);
	SDL_unlockSurface(screen);
}

void Vid_Shutdown ()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
