#include "SDL.h"
#include "sys.h"
#include "game.h"
#include "sys/param.h"
#include "vid.h"

#define BASEWIDTH	640
#define BASEHEIGHT	480
#define BASEDEPTH	24

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x00000000
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0x00000000
#endif

static SDL_Surface * screen;

void Vid_Init ()
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO))
		Sys_Error("Vid: SDL_InitSubSysten failed!");
	
	if (!(screen = SDL_SetVideoMode(BASEWIDTH, BASEHEIGHT, BASEDEPTH, SDL_SWSURFACE)))
		Sys_Error("Vid: Couldn't set video mode: %s", SDL_GetError());

	SDL_WM_SetCaption(GAME_NAME, GAME_NAME);
	SDL_ShowCursor(0);
}

void * Vid_CreateRGBSurface (uint width, uint height)
{
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, BASEDEPTH, rmask, gmask, bmask, amask);
}

void * Vid_CreateYUVSurface (uint width, uint height)
{
	return SDL_CreateYUVOverlay(width, height, SDL_IYUV_OVERLAY, screen);
}

void * Vid_GetAndLockRGBBuffer (void * ref)
{
	SDL_Surface * tmp = ref;
	SDL_LockSurface(tmp);
	return tmp->pixels;
}

void * Vid_GetAndLockYUVBuffer (void * ref)
{
	SDL_Overlay * tmp = ref;
	SDL_LockYUVOverlay(tmp);
	return *tmp->pixels;
}

void Vid_UnlockRGBBuffer (void * ref)
{
	SDL_Surface * tmp = ref;
	SDL_UnlockSurface(tmp);
}

void Vid_UnlockYUVBuffer (void * ref)
{
	SDL_Overlay * tmp = ref;
	SDL_UnlockYUVOverlay(tmp);
}

void Vid_BlitRGBBuffer(void * ref, uint x, uint y)
{
	SDL_Surface * tmp = ref;
	//SDL_Rect source = { 0, 0, tmpsurf->w, tmpsurf->h};
	SDL_Rect target = { x, y, tmp->w, tmp->h};
	SDL_BlitSurface(tmp, NULL, screen, &target);
}

void Vid_BlitYUVBuffer(void * ref, uint x, uint y)
{
	SDL_Overlay * tmp = ref;
	SDL_Rect overlaytarget = { x, y, tmp->w, tmp->h};
	SDL_DisplayYUVOverlay(tmp, &overlaytarget);
}

void Vid_Blank()
{
	SDL_UnlockSurface(screen);
	memset(screen->pixels, 0x00, BASEWIDTH*BASEHEIGHT*3);
}

void Vid_Update ()
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void Vid_Shutdown ()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
