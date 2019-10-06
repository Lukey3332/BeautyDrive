#include "key.h"
#include "SDL.h"
#include "sys.h"

void In_Init ()
{

}

void In_Shutdown ()
{

}

int In_SendEvents ()
{
	SDL_Event event;
	int status = SDL_PollEvent(&event);
	do {
		HandleKey(event.key.keysym.sym, (event.type == SDL_KEYDOWN ? 1 : 0));
	} while (SDL_PollEvent(&event));
	return status;
}
