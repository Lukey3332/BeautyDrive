#include "SDL.h"
#include "sys.h"
#include "game.h"
#include "sys/param.h"
#include "vid.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0xff000000
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

surface * Vid_CreateSurface (int format, int features, uint width, uint height)
{
	switch(format) {
		case RGB:
			return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, rmask, gmask, bmask, 0x00000000);
		break;
		case ARGB:
			return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
		break;
		case YUV:
			return SDL_CreateYUVOverlay(width, height, SDL_IYUV_OVERLAY, screen);
		break;
		
		default:
			abort();
	}
}

surface * Vid_GetAndLockBuffer (int format, surface * ref)
{
	if( format != YUV ) {
		SDL_Surface * tmp_surf = ref;
		SDL_LockSurface(tmp_surf);
		return tmp_surf->pixels;
	} else {
		SDL_Overlay * tmp_ovl = ref;
		SDL_LockYUVOverlay(tmp_ovl);
		return *tmp_ovl->pixels;
	}
}

void Vid_UnlockBuffer (int format, surface * ref)
{
	if( format != YUV ) {
		SDL_UnlockSurface((SDL_Surface *) ref);
	} else {
		SDL_UnlockYUVOverlay((SDL_Overlay *) ref);
	}
}

void Vid_BlitBuffer(int format, surface * ref, uint x, uint y)
{
	if( format != YUV ) {
		SDL_Surface * tmp_surf = ref;
		SDL_Rect target = { x, y, tmp_surf->w, tmp_surf->h};
		SDL_BlitSurface(tmp_surf, NULL, screen, &target);
	} else {
		SDL_Overlay * tmp_ovl = ref;
		SDL_Rect overlaytarget = { x, y, tmp_ovl->w, tmp_ovl->h};
		SDL_DisplayYUVOverlay(tmp_ovl, &overlaytarget);
	}
}

void Vid_BlitBufferScaled(int format, surface * ref, uint x, uint y, uint w, uint h)
{
	if( format != YUV ) {
		SDL_Surface * tmp_surf = ref;
		SDL_Rect target = { x, y, tmp_surf->w, tmp_surf->h};
		SDL_BlitSurface(tmp_surf, NULL, screen, &target);
	} else {
		SDL_Overlay * tmp_ovl = ref;
		SDL_Rect overlaytarget = { x, y, tmp_ovl->w, tmp_ovl->h};
		SDL_DisplayYUVOverlay(tmp_ovl, &overlaytarget);
	}
}

void Vid_Blank()
{
	SDL_UnlockSurface(screen);
	memset(screen->pixels, 0x00, BASEWIDTH*BASEHEIGHT*(BASEDEPTH/8));
}

void Vid_Update ()
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void Vid_Shutdown ()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
