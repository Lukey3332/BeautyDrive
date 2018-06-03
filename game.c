// game.c
#include "sys.h"
#include "dec.h"
#include "key.h"
#include "util.h"
#include "game.h"
#include "vid.h"
#include "SDL.h"
#include "in.h"
#include "map.h"

static uint prevtime; //Time of the previous run
void * car;
vectorT playerPos;

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
	Map_Load("../BeautyDrive-data/candy.obj");
	Map_GetStartingPos(&playerPos, 0);
	if(Sys_Args.argc==2)
		Dec_LoadBackground(Sys_Args.argv[1]);
	else
		Dec_LoadBackground(NULL);
	car = Vid_CreateRGBSurface( 10, 20, STATIC);
	//memset(Vid_GetAndLockRGBBuffer(car), 0xff, 10*20*3);
	//Vid_UnlockRGBBuffer(car);
	prevtime = Sys_Time();
}

void Game_Frame ()
{
	Sys_Sleep( (uint) MAX( (int) (33-(Sys_Time()-prevtime)), (int) 0) );
	prevtime = Sys_Time();
	In_SendEvents();
	playerPos.x += speed*0.1;
	playerPos.y += steer*0.1;
	
	Sys_Printf( "Frame: %d (X: %f Y: %f)", Map_ToFrame( playerPos, 200), playerPos.x, playerPos.y);
	
	Vid_Blank();
	if(speed) {
		if(Dec_Advance())
			Dec_Seek( REWIND, 0);
	}
	
	Dec_DrawBackground();
	//Vid_BlitRGBBuffer(car, cary, 400);
	Vid_Update();
}

void Game_Shutdown ()
{
	
}
