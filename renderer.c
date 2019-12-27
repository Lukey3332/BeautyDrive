// renderer.c

#include "vector.h"
#include "map.h"
#include "vid.h"
#include <math.h>

void Render_Object ( objectT * obj, vec4_t * cam, uint frame )
{
	vectorT tmp_euler, angles, projection;
	vec4_t cam_tmp = *cam;
	vec4_t tmp = obj->point.orientation;
	uint width = obj->renderdata->width;
	uint height = obj->renderdata->height;
	uint center_x = obj->renderdata->center_x;
	uint center_y = obj->renderdata->center_y;
	surface ** images = obj->renderdata->images;

	Map_Project( obj->point.pos, &projection, frame );


	Vec_QuatToEuler( &tmp, &tmp_euler );
	tmp_euler.y = 0.0;
	tmp_euler.z = 0.0;
	Vec_EulerToQuat( &tmp_euler, &tmp );
	Vec_QuatToEuler( &cam_tmp, &tmp_euler );
	tmp_euler.y = 0.0;
	tmp_euler.z = 0.0;
	Vec_EulerToQuat( &tmp_euler, &cam_tmp );
	Vec_InvertQuat( &cam_tmp );
	Vec_MulQuat( &tmp, &cam_tmp );
	Vec_QuatToEuler( &tmp, &angles );

	int proj_rotation = (projection.x-(BASEWIDTH/2))/20;
	double rotation = (angles.x * (180.0 / M_PI));
	rotation += 90.0 + proj_rotation;
	rotation = rotation - ( floor(rotation / 360.0) * 360 );

	double adjusted_z = 1.0 - projection.z;
	adjusted_z *= 40.0;

	if(projection.z < 1.0){
		Vid_BlitBufferScaled( ARGB, images[(uint)rotation/2], \
									projection.x-(center_x * adjusted_z), \
									projection.y-(center_y * adjusted_z), \
									width * adjusted_z, height * adjusted_z);
	}
}
