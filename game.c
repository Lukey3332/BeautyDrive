// game.c
#include "sys.h"
#include "dec.h"
#include "vid.h"
#include "key.h"
#include "game.h"
#include "SDL.h"
#include "in.h"

static uint prevtime; //Time of the previous run

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
	if(sys_params.argc==2)
		Dec_LoadBackground(sys_params.argv[1]);
	else
		Dec_LoadBackground(NULL);
	Dec_DrawBackground();
}

void Game_Frame ()
{
	Sys_Sleep( 16-(prevtime-Sys_Time()) );
	In_SendEvents();
	if(speed)
		Dec_DrawBackground();
	Vid_Update();
	prevtime = Sys_Time();
}

void Game_Shutdown ()
{
	
}
