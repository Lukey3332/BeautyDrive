#include "vector.h"
#include "sys.h"
#include "util.h"
#include <math.h>
#include <GL/glu.h>

float Vec_LineToVecDistance( vectorT v, vectorT w, vectorT point)
{
	const float l2 = VEC_DISTANCE_SQUARED( v, w);
	if (l2 == 0.0) return VEC_DISTANCE(point, v);
	float t = ((point.x - v.x) * (w.x - v.x) + (point.y - v.y) * (w.y - v.y) + (point.z - v.z) * (w.z - v.z)) / l2;
	t = MAX(0, MIN(1, t));
	vectorT proj;
	proj.x = v.x + t * (w.x - v.x);
	proj.y = v.y + t * (w.y - v.y);
	proj.z = v.z + t * (w.z - v.z);
	return VEC_DISTANCE(point, proj);
}

float Vec_LineToVecProjection( vectorT v, vectorT w, vectorT point)
{
	return ((point.x - v.x) * (w.x - v.x) + (point.y - v.y) * (w.y - v.y) + (point.z - v.z) * (w.z - v.z)) / VEC_DISTANCE_SQUARED( v, w);
}

int Vec_Project( vectorT pos, double * x, double * y, double * z, double * viewmodel, double * projection, int * viewport)
{
	return gluProject( pos.x, pos.y, pos.z, viewmodel, projection, viewport, x, y, z);
}

