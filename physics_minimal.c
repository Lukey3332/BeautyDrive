// physics_minimal.c

#include "map.h"
#include <math.h>

void Physics_Update_Player ( objectT * player, float gas, float steer )
{
	{
		vectorT euler = { steer*-5.0/(180.0 / M_PI), 0.0, 0.0 };
		vec4_t quat;
		Vec_EulerToQuat( &euler, &quat );
		Vec_MulQuat( &player->point.orientation, &quat );
	}

	{
		vectorT tmp_vec = { 1.0, 0.0, 0.0 };
		Vec_MulVecQuat( &tmp_vec, &player->point.orientation );
		player->point.pos.x += tmp_vec.x * gas * 0.2;
		player->point.pos.y += tmp_vec.y * gas * 0.2;
	}
}
