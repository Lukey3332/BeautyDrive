// game.c
#include "sys.h"
#include "dec.h"
#include "key.h"
#include "util.h"
#include "game.h"
#include "vid.h"
#include "SDL.h"
#include "in.h"

static uint prevtime; //Time of the previous run
void * car;
uint carx, cary = 0;

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
	car = Vid_CreateRGBSurface( 10, 20);
	memset(Vid_GetAndLockRGBBuffer(car), 0xff, 10*20*3);
	Vid_UnlockRGBBuffer(car);
	prevtime = Sys_Time();
}

void Game_Frame ()
{
	Sys_Sleep( (uint) MAX( (int) (33-(Sys_Time()-prevtime)), (int) 0) );
	prevtime = Sys_Time();
	In_SendEvents();
	cary += steer;
	
	Vid_Blank();
	if(speed) {
		Dec_DrawBackground();
	}
	Vid_UpdateRGBBuffer(car, cary, 40);
	Vid_Update();
	prevtime = Sys_Time();
}

void Game_Shutdown ()
{
	
}
