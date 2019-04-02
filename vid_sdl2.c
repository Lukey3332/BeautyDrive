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

surface * Vid_CreateSurface (int format, int features, uint width, uint height)
{
	int sdl_format;
	switch(format) {
		case RGB:
			sdl_format = SDL_PIXELFORMAT_RGB888;
		break;
		case ARGB:
			sdl_format = SDL_PIXELFORMAT_ARGB8888;
		break;
		case YUV:
			sdl_format = SDL_PIXELFORMAT_IYUV;
		break;
		
		default:
			abort();
	}
	int sdl_update = 0;
	if( features & UPDATE_STATIC ) sdl_update = SDL_TEXTUREACCESS_STATIC;
	else sdl_update = SDL_TEXTUREACCESS_STREAMING;
	SDL_Texture * texture = SDL_CreateTexture( renderer, sdl_format, sdl_update, width, height );
	if( features & BLEND_ALPHA || features & BLEND_KEY ) SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND);
	return texture;
}

surface * Vid_GetAndLockBuffer (int format, surface * ref)
{
	SDL_Texture * tmp = ref;
	surface * ret;
	int discard;
	SDL_LockTexture(tmp, NULL, &ret, &discard);
	return ret;
}

void Vid_UnlockBuffer (int format, surface * ref)
{
	SDL_Texture * tmp = ref;
	SDL_UnlockTexture(tmp);
}

void Vid_BlitBuffer (int format, surface * ref, uint x, uint y)
{
	SDL_Texture * tmp = ref;
	int w, h;
	SDL_QueryTexture( tmp, NULL, NULL, &w, &h);
	SDL_Rect target = { x, y, w, h};
	SDL_RenderCopy(renderer, tmp, NULL, &target);
}

void Vid_BlitBufferScaled (int format, surface * ref, uint x, uint y, uint w, uint h)
{
	SDL_Texture * tmp = ref;
	SDL_Rect target = { x, y, w, h};
	SDL_RenderCopy(renderer, tmp, NULL, &target);
}

void Vid_Blank ()
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
