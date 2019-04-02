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
#include "vector.h"
#include <math.h>

static uint prevTime; //Time of the previous run
surface * Car_Images[180];
object Point, Player;

void Game_Init ()
{
	map_key(SDLK_UP, CMD_ACCEL);
	map_key(SDLK_DOWN, CMD_BRAKE);
	map_key(SDLK_ESCAPE, CMD_QUIT);
	map_key(SDLK_LEFT, CMD_LEFT);
	map_key(SDLK_RIGHT, CMD_RIGHT);
	if(Map_Load("../BeautyDrive-data/candy.obj")) Sys_Error("Error in Map_Load!");
	Map_GetStartingPos(&Player, 0);
	Map_GetStartingPos(&Point, 1);
	if(Sys_Args.argc==2)
		Dec_LoadBackground(Sys_Args.argv[1]);
	else
		Dec_LoadBackground(NULL);
	
	char buf[128];
	for( uint n = 0; n<180; n++ ){
		sprintf( buf, "../BeautyDrive-data/car/%04u.png", n);
		Car_Images[n] = Map_LoadPNG( buf );
	}
	
	prevTime = Sys_Time();
}

void Game_Frame ()
{
	vectorT car_proj, player_orientation, player_angles;
	vec4_t cam;
	Sys_Sleep( (uint) MAX( (int) (33-(Sys_Time()-prevTime)), (int) 0) );
	prevTime = Sys_Time();
	
	In_SendEvents();
	
	vectorT euler = { steer*0.05, 0.0, 0.0 };
	vec4_t quat;
	Vec_EulerToQuat( &euler, &quat );
	Vec_MulQuat( &Player.orientation, &quat );
	Vec_QuatToEuler( &Player.orientation, &player_orientation );
	double tmp_mat[16];
	Vec_IdentityMat( tmp_mat );
	vectorT tmp_vec = { 0.0, 0.0, 1.0 };
	Vec_QuatToMat( &Player.orientation, &tmp_mat );
	Vec_MulVecMat( &tmp_vec, &tmp_mat );
	Player.pos.x += tmp_vec.x * speed * 0.2;
	Player.pos.y += tmp_vec.z * speed * 0.2;
	//Player.pos.x += sin(player_orientation.y)*speed*0.1;
	//Player.pos.y += cos(player_orientation.y)*speed*0.1;
	uint frame = MAX( 0, (int)Map_ToFrame( Player.pos ) );
	frame = Dec_Update( frame );
	Map_Project( Player.pos, &car_proj, frame );
	Map_CameraOrientation( &cam, frame );
	Vec_MulQuat( &cam, &Player.orientation );
	Vec_QuatToEuler( &cam, &player_angles );
	
	Sys_Printf( "Frame: %d (X: %f Y: %f Z: %f) \nAngles: %f, %f, %f", frame, Player.pos.x, Player.pos.y, Player.pos.z, player_orientation.x * (180.0 / M_PI), player_orientation.y * (180.0 / M_PI), player_orientation.z * (180.0 / M_PI));
	Sys_Printf( "Projection: X: %f Y: %f Z: %f \nAngles: %f, %f, %f", frame, car_proj.x, car_proj.y, car_proj.z, player_angles.x * (180.0 / M_PI), player_angles.y * (180.0 / M_PI), player_angles.z * (180.0 / M_PI));
	
	Vid_Blank();
	Dec_DrawBackground();
	int proj_rotation = (car_proj.x-320)/20;
	double rotation = 360.0-(player_angles.z * (180.0 / M_PI)) + proj_rotation;
	rotation = rotation - floor(rotation / 360.0) * 360.0;
	
	double adjusted_z = 1.0 - car_proj.z;
	adjusted_z *= 40.0;
	if(car_proj.z < 1.0) Vid_BlitBufferScaled( ARGB, Car_Images[(uint)rotation/2], car_proj.x-160, car_proj.y-200, 320 * adjusted_z, 240 * adjusted_z);
	Vid_Update();
}

void Game_Shutdown ()
{
	
}
