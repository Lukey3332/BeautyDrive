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

static uint prevTime; //Time of the previous run
void * car;
void * point_surf;
vectorT playerPos, point;

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
	if(Map_Load("../BeautyDrive-data/candy.obj")) Sys_Error("Error in Map_Load!");
	Map_GetStartingPos(&playerPos, 0);
	if(Sys_Args.argc==2)
		Dec_LoadBackground(Sys_Args.argv[1]);
	else
		Dec_LoadBackground(NULL);
	car = Vid_CreateRGBSurface( 10, 10, STATIC);
	point_surf = Vid_CreateRGBSurface( 20, 20, STATIC);
	point.x = 10.0;
	point.y = 0.0;
	point.z = 1.0;
	//memset(Vid_GetAndLockRGBBuffer(car), 0xff, 10*20*3);
	//Vid_UnlockRGBBuffer(car);
	prevTime = Sys_Time();
}

void Game_Frame ()
{
	vectorT car_proj, point_proj;
	Sys_Sleep( (uint) MAX( (int) (33-(Sys_Time()-prevTime)), (int) 0) );
	prevTime = Sys_Time();
	
	In_SendEvents();
	
	playerPos.x += speed*0.1;
	playerPos.y += steer*-0.1;
	uint frame = MAX( 0, (int)Map_ToFrame( playerPos )-20);
	Map_Project( playerPos, &car_proj, frame );
	Map_Project( point, &point_proj, frame );
	
	Sys_Printf( "Frame: %d (X: %f Y: %f Z: %f) \nProjection: %f, %f, %f", frame, playerPos.x, playerPos.y, playerPos.z, car_proj.x, car_proj.y, car_proj.z);
	
	Vid_Blank();
	if(Dec_Update( frame )) Dec_Seek( REWIND, 0 );
	Dec_DrawBackground();
	Vid_BlitRGBBuffer(car, car_proj.x, car_proj.y);
	Vid_BlitRGBBuffer(point_surf, point_proj.x, point_proj.y);
	Vid_Update();
}

void Game_Shutdown ()
{
	
}
