#include "key.h"
#include "SDL.h"
#include "sys.h"

void In_Init ()
{

}

void In_Shutdown ()
{

}

void In_SendEvents ()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		HandleKey(event.key.keysym.sym, (event.key.type == SDL_KEYDOWN ? 1 : 0));
	}
}
