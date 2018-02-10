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
#define amask 0x000000ff
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0xff000000
#endif

struct {
	uint count;
	struct {
		Vid_SurfaceType type;
		union {
			SDL_Surface * rgb;
			SDL_Overlay * yuv;
		} ptr;
	} surface[32];
} surfaces;

static SDL_Surface * screen;

void Vid_Init ()
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO))
		Sys_Error("Vid: SDL_InitSubSysten failed!");
	
	if (!(screen = SDL_SetVideoMode(BASEWIDTH, BASEHEIGHT, BASEDEPTH, SDL_SWSURFACE)))
		Sys_Error("Vid: Couldn't set video mode: %s", SDL_GetError());
	
	surfaces.surface[0].type = RGB;
	surfaces.surface[0].ptr.rgb = screen;
	surfaces.count = 1;
	SDL_WM_SetCaption(GAME_NAME, GAME_NAME);
	SDL_ShowCursor(0);
}

Vid_SurfaceRef Vid_CreateSurface (uint width, uint height, Vid_SurfaceType type)
{
	uint count = surfaces.count;
	surfaces.count++;
	switch (type) {
		case RGB:
			surfaces.surface[count].type = RGB;
			surfaces.surface[count].ptr.rgb = SDL_CreateRGBSurface(0, width, height, BASEDEPTH, rmask, gmask, bmask, amask);
			return count;
		case YUV:
			surfaces.surface[count].type = YUV;
			surfaces.surface[count].ptr.yuv = SDL_CreateYUVOverlay(width, height, SDL_IYUV_OVERLAY, screen);
			return count;
	}
	
}

void * Vid_GetAndLockBuffer (Vid_SurfaceRef ref) {
	SDL_Surface * tmpsurf;
	SDL_Overlay * tmplay;
	switch (surfaces.surface[ref].type) {
		case RGB:
			tmpsurf = surfaces.surface[ref].ptr.rgb;
			SDL_LockSurface(tmpsurf);
			return tmpsurf->pixels;
		case YUV:
			tmplay = surfaces.surface[ref].ptr.yuv;
			SDL_LockYUVOverlay(tmplay);
			return *tmplay->pixels;
	}
}

void Vid_UnlockBuffer (Vid_SurfaceRef ref) {
	SDL_Surface * tmpsurf;
	SDL_Overlay * tmplay;
	switch (surfaces.surface[ref].type) {
		case RGB:
			tmpsurf = surfaces.surface[ref].ptr.rgb;
			SDL_UnlockSurface(tmpsurf);
			return;
		case YUV:
			tmplay = surfaces.surface[ref].ptr.yuv;
			SDL_UnlockYUVOverlay(tmplay);
			SDL_Rect target = { 0, 0, BASEWIDTH, BASEHEIGHT};
			SDL_DisplayYUVOverlay(tmplay, &target);
			return;
	}
}

void Vid_Update (void * buffer, uint size)
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void Vid_Shutdown ()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
