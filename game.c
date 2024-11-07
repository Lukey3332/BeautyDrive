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
#include "renderer.h"
#include "physics.h"
#include "vector.h"
#include <math.h>

static uint prevTime; //Time of the previous run
objectT Point, Point2, Player;

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
	if(Map_Load("data/lazy.obj")){
		Sys_Error("Error in Map_Load!");
	}
	Map_GetStartingPos(&Player.point, 0);
	Map_GetStartingPos(&Point.point, 1);
	Map_GetStartingPos(&Point2.point, 2);
	if(Sys_Args.argc==2)
		Dec_LoadBackground(Sys_Args.argv[1]);
	else
		Dec_LoadBackground(NULL);

	renderdataT * renderdata = malloc( sizeof(renderdataT) );
	Map_LoadRenderdata( renderdata, "data/car" );
	Player.renderdata = renderdata;
	Point.renderdata = renderdata;
	Point2.renderdata = renderdata;
	Player.velocity.x = 0.0;

	prevTime = Sys_Time();
}

void Game_Frame ()
{
	Sys_Sleep( (uint) MAX( (int) (33-(Sys_Time()-prevTime)), (int) 0) );
	prevTime = Sys_Time();

	In_SendEvents();
	Physics_Update_Player( &Player, speed, steer );

	uint frame = MAX( 0, (int)Map_ToFrame( Player.point.pos ) );
	frame = Dec_Update( frame );

	Sys_Printf( "Frame: %d (X: %f Y: %f Z: %f)", frame, Player.point.pos.x, \
														Player.point.pos.y, \
														Player.point.pos.z);

	vec4_t cam;
	Vid_Blank();
	Dec_DrawBackground();
	Map_CameraOrientation( &cam, frame );
	Render_Object( &Player, &cam, frame );
	Render_Object( &Point, &cam, frame );
	Render_Object( &Point2, &cam, frame );
	Vid_Update();
}

void Game_Shutdown ()
{
	
}
