#include "SDL.h"
#include "sys.h"
#include "game.h"
#include "sys/param.h"
#include "vid.h"

static SDL_Window * window = NULL;
static SDL_Renderer * renderer = NULL;

void Vid_Init ()
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO))
		Sys_Error("Vid: SDL_InitSubSysten failed!");
	
	if(!(window = SDL_CreateWindow( GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, BASEWIDTH, BASEHEIGHT, SDL_WINDOW_SHOWN )))
		Sys_Error("Vid: Could not create Window: %s", SDL_GetError());

	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_ShowCursor(0);
}

void * Vid_CreateRGBSurface (uint width, uint height, int update)
{
	int type;
	switch(update) {
		case STATIC:
			type = SDL_TEXTUREACCESS_STATIC;
		break;
		
		case STREAMING:
			type = SDL_TEXTUREACCESS_STREAMING;
		break;
		
		default:
			type = SDL_TEXTUREACCESS_STREAMING;
	}
	SDL_Texture * ret = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, type, width, height );
	SDL_SetTextureBlendMode( ret, SDL_BLENDMODE_BLEND);
	return ret;
}

void * Vid_CreateYUVSurface (uint width, uint height, int update)
{
	int type;
	switch(update) {
		case STATIC:
			type = SDL_TEXTUREACCESS_STATIC;
		break;
		
		case STREAMING:
			type = SDL_TEXTUREACCESS_STREAMING;
		break;
		
		default:
			type = SDL_TEXTUREACCESS_STREAMING;
	}
	return SDL_CreateTexture( renderer, SDL_PIXELFORMAT_IYUV, type, width, height );
}

void * Vid_GetAndLockRGBBuffer (void * ref)
{
	SDL_Texture * tmp = ref;
	void * ret;
	int discard;
	SDL_LockTexture(tmp, NULL, &ret, &discard);
	return ret;
}

void * Vid_GetAndLockYUVBuffer (void * ref)
{
	SDL_Texture * tmp = ref;
	void * ret;
	int discard;
	SDL_LockTexture(tmp, NULL, &ret, &discard);
	return ret;
}

void Vid_UnlockRGBBuffer (void * ref)
{
	SDL_Texture * tmp = ref;
	SDL_UnlockTexture(tmp);
}

void Vid_UnlockYUVBuffer (void * ref)
{
	SDL_Texture * tmp = ref;
	SDL_UnlockTexture(tmp);
}

void Vid_BlitRGBBuffer(void * ref, uint x, uint y)
{
	SDL_Texture * tmp = ref;
	int w, h;
	SDL_QueryTexture( tmp, NULL, NULL, &w, &h);
	SDL_Rect target = { x, y, w, h};
	SDL_RenderCopy(renderer, tmp, NULL, &target);
}

void Vid_BlitYUVBuffer(void * ref, uint x, uint y)
{
	SDL_Texture * tmp = ref;
	int w, h;
	SDL_QueryTexture( tmp, NULL, NULL, &w, &h);
	SDL_Rect target = { x, y, w, h};
	SDL_RenderCopy(renderer, tmp, NULL, &target);
}

void Vid_Blank()
{
	SDL_RenderClear(renderer);
}

void Vid_Update ()
{
	SDL_RenderPresent(renderer);
}

void Vid_Shutdown ()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
