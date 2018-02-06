// game.c
#include "key.h"
#include "game.h"
#include "SDL.h"
#include "in.h"

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
}

void Game_Frame ()
{
	In_SendEvents();
}

void Game_Shutdown ()
{
	
}
