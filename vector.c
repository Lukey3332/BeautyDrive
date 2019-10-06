#include "vector.h"
#include "sys.h"
#include "util.h"
#include <math.h>
#include <string.h>
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

int Vec_MatrixToAxisAngle( double * m, vec4_t * output )
{
	double angle,x,y,z; // variables for result
	double epsilon = 0.01; // margin to allow for rounding errors
	double epsilon2 = 0.1; // margin to distinguish between 0 and 180 degrees

	if ((fabs(m[M(0,1)]-m[M(1,0)])< epsilon)
	  && (fabs(m[M(0,2)]-m[M(2,0)])< epsilon)
	  && (fabs(m[M(1,2)]-m[M(2,1)])< epsilon)) {
		// singularity found
		// first check for identity matrix which must have +1 for all terms
		//  in leading diagonaland zero in other terms
		if ((fabs(m[M(0,1)]+m[M(1,0)]) < epsilon2)
		  && (fabs(m[M(0,2)]+m[M(2,0)]) < epsilon2)
		  && (fabs(m[M(1,2)]+m[M(2,1)]) < epsilon2)
		  && (fabs(m[M(0,0)]+m[M(1,1)]+m[M(2,2)]-3) < epsilon2)) {
			// this singularity is identity matrix so angle = 0
			output->x = 1.0;
			output->y = 0.0;
			output->z = 0.0;
			output->w = 0.0;
			return 1; // zero angle, arbitrary axis
		}
		// otherwise this singularity is angle = 180
		angle = M_PI;
		double xx = (m[M(0,0)]+1)/2;
		double yy = (m[M(1,1)]+1)/2;
		double zz = (m[M(2,2)]+1)/2;
		double xy = (m[M(0,1)]+m[M(1,0)])/4;
		double xz = (m[M(0,2)]+m[M(2,0)])/4;
		double yz = (m[M(1,2)]+m[M(2,1)])/4;
		if ((xx > yy) && (xx > zz)) { // m[0][0] is the largest diagonal term
			if (xx< epsilon) {
				x = 0;
				y = 0.7071;
				z = 0.7071;
			} else {
				x = sqrt(xx);
				y = xy/x;
				z = xz/x;
			}
		} else if (yy > zz) { // m[1][1] is the largest diagonal term
			if (yy< epsilon) {
				x = 0.7071;
				y = 0;
				z = 0.7071;
			} else {
				y = sqrt(yy);
				x = xy/y;
				z = yz/y;
			}	
		} else { // m[2][2] is the largest diagonal term so base result on this
			if (zz< epsilon) {
				x = 0.7071;
				y = 0.7071;
				z = 0;
			} else {
				z = sqrt(zz);
				x = xz/z;
				y = yz/z;
			}
		}
		output->x = x;
		output->y = y;
		output->z = z;
		output->w = angle;
		return 0; // return 180 deg rotation
	}
	// as we have reached here there are no singularities so we can handle normally
	double s = sqrt((m[M(2,1)] - m[M(1,2)])*(m[M(2,1)] - m[M(1,2)])
		+(m[M(0,2)] - m[M(2,0)])*(m[M(0,2)] - m[M(2,0)])
		+(m[M(1,0)] - m[M(0,1)])*(m[M(1,0)] - m[M(0,1)])); // used to normalise
	if (fabs(s) < 0.001) s=1; 
		// prevent divide by zero, should not happen if matrix is orthogonal and should be
		// caught by singularity test above, but I've left it in just in case
	angle = acos(( m[M(0,0)] + m[M(1,1)] + m[M(2,2)] - 1)/2);
	x = (m[M(2,1)] - m[M(1,2)])/s;
	y = (m[M(0,2)] - m[M(2,0)])/s;
	z = (m[M(1,0)] - m[M(0,1)])/s;
	output->x = x;
	output->y = y;
	output->z = z;
	output->w = angle;
	return 0;
}

void Vec_AxisAngleToQuaternion( vec4_t * var )
{
	double s = sin(var->w/2);
	var->x = var->x * s;
	var->y = var->y * s;
	var->z = var->z * s;
	var->w = cos(var->w/2);
}

void Vec_QuatToEuler( vec4_t * q, vectorT * result )
{
	double sinr = 2.0 * (q->w * q->x + q->y * q->z);
	double cosr = 1.0 - 2.0 * (q->x * q->x + q->y * q->y);
	result->z = atan2(sinr, cosr);

	// pitch (y-axis rotation)
	double sinp = 2.0 * (q->w * q->y - q->z * q->x);
	if (fabs(sinp) >= 1)
		result->y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		result->y = asin(sinp);

	// yaw (z-axis rotation)
	double siny = 2.0 * (q->w * q->z + q->x * q->y);
	double cosy = 1.0 - 2.0 * (q->y * q->y + q->z * q->z);  
	result->x = atan2(siny, cosy);
}

void Vec_EulerToQuat( vectorT * euler, vec4_t * result )
{
	// Assuming the angles are in radians.
	double c1 = cos(euler->y/2);
	double s1 = sin(euler->y/2);
	double c2 = cos(euler->x/2);
	double s2 = sin(euler->x/2);
	double c3 = cos(euler->z/2);
	double s3 = sin(euler->z/2);
	double c1c2 = c1*c2;
	double s1s2 = s1*s2;
	result->w =c1c2*c3 - s1s2*s3;
	result->x =c1c2*s3 + s1s2*c3;
	result->y =s1*c2*c3 + c1*s2*s3;
	result->z =c1*s2*c3 - s1*c2*s3;
}

void Vec_MulQuat( vec4_t * a, vec4_t * b )
{
	float x =  a->x * b->w + a->y * b->z - a->z * b->y + a->w * b->x;
	float y = -a->x * b->z + a->y * b->w + a->z * b->x + a->w * b->y;
	float z =  a->x * b->y - a->y * b->x + a->z * b->w + a->w * b->z;
	float w = -a->x * b->x - a->y * b->y - a->z * b->z + a->w * b->w;
	a->x = x;
	a->y = y;
	a->z = z;
	a->w = w;
}

void Vec_NomalizeQuat( vec4_t * q )
{
	const float n = 1.0f/sqrt(q->x*q->x+q->y*q->y+q->z*q->z+q->w*q->w);
	q->x *= n;
	q->y *= n;
	q->z *= n;
	q->w *= n;
}

void Vec_InvertQuat( vec4_t * q )
{
	const float norm = q->x*q->x+q->y*q->y+q->z*q->z+q->w*q->w;
	
	// Prevent Divide by Zero
	if( norm < 0.001 ){
		return;
	}
	
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
	
	if( norm > 0.999 ){
		return;
	}
	
	q->x /= norm;
	q->y /= norm;
	q->z /= norm;
	q->w /= norm;
}

void Vec_QuatToMat( vec4_t * q, double * m )
{
	m[M(0,0)] = 1.0 - 2.0*q->y*q->y - 2.0*q->z*q->z;
	m[M(0,1)] = 2.0*q->x*q->y - 2.0*q->z*q->w;
	m[M(0,2)] = 2.0*q->x*q->z + 2.0*q->y*q->w;
	m[M(0,3)] = 0.0;
	m[M(1,0)] = 2.0*q->x*q->y + 2.0*q->z*q->w;
	m[M(1,1)] = 1.0 - 2.0*q->x*q->x - 2.0*q->z*q->z;
	m[M(1,2)] = 2.0*q->y*q->z - 2.0*q->x*q->w;
	m[M(1,3)] = 0.0;
	m[M(2,0)] = 2.0*q->x*q->z - 2.0*q->y*q->w;
	m[M(2,1)] = 2.0*q->y*q->z + 2.0*q->x*q->w;
	m[M(2,2)] = 1.0 - 2.0*q->x*q->x - 2.0*q->y*q->y;
	m[M(2,3)] = 0.0;
	m[M(3,0)] = 0.0;
	m[M(3,1)] = 0.0;
	m[M(3,2)] = 0.0;
	m[M(3,3)] = 1.0;
}

void Vec_MulVecQuat(vectorT * vec, vec4_t * q)
{
	float num12 = q->x + q->x;
	float num2 = q->y + q->y;
	float num = q->z + q->z;
	float num11 = q->w * num12;
	float num10 = q->w * num2;
	float num9 = q->w * num;
	float num8 = q->x * num12;
	float num7 = q->x * num2;
	float num6 = q->x * num;
	float num5 = q->y * num2;
	float num4 = q->y * num;
	float num3 = q->z * num;
	float num15 = ((vec->x * ((1.0f - num5) - num3)) + (vec->y * (num7 - num9))) + (vec->z * (num6 + num10));
	float num14 = ((vec->x * (num7 + num9)) + (vec->y * ((1.0f - num8) - num3))) + (vec->z * (num4 - num11));
	float num13 = ((vec->x * (num6 - num10)) + (vec->y * (num4 + num11))) + (vec->z * ((1.0f - num8) - num5));
	vec->x = num15;
	vec->y = num14;
	vec->z = num13;
}

void Vec_AddMat( double * ma, double * mb )
{
	double result[16] = { 0.0 };
	for(uint n = 0; n < 16; n++) result[n] = ma[n] + mb[n];
	memcpy( ma, result, sizeof(double) * 16 );
}

void Vec_MulMat( double * ma, double * mb )
{
	double result[16] = { 0.0 };
	for(uint i=0; i<4; i++)
		for(uint j=0; j<4; j++)
			for(uint k=0; k<4; k++)
				result[M(i,j)]+=ma[M(i,k)]*mb[M(k,j)];
	
	memcpy( ma, result, sizeof(double) * 16 );
}

void Vec_MulVecMat( vectorT * vec, double * m )
{
	vectorT result;
	result.x = m[M(0,0)] * vec->x + m[M(0,1)] * vec->y + m[M(0,2)] * vec->z + m[M(0,3)];
	result.y = m[M(1,0)] * vec->x + m[M(1,1)] * vec->y + m[M(1,2)] * vec->z + m[M(1,3)];
	result.z = m[M(2,0)] * vec->x + m[M(2,1)] * vec->y + m[M(2,2)] * vec->z + m[M(2,3)];
	memcpy( vec, &result, sizeof(vectorT) );
}

void Vec_IdentityMat( double * m )
{
	for( uint n = 0; n < 16; n++) m[n] = 0.0;
	m[M(0,0)] = 1.0;
	m[M(1,1)] = 1.0;
	m[M(2,2)] = 1.0;
	m[M(3,3)] = 1.0;
}
