
#ifndef VECTOR_H
#define VECTOR_H

typedef struct { float x; float y; float z; } vectorT;
typedef struct { float x; float y; float z; float w; } vec4_t;

#define VEC_DISTANCE( a, b ) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) )
#define VEC_DISTANCE_SQUARED( a, b ) ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z))
#define M(x, y) ( (x) * 4 + (y) )

float Vec_LineToVecDistance( vectorT v, vectorT w, vectorT point);
float Vec_LineToVecProjection( vectorT v, vectorT w, vectorT point);
int Vec_Project( vectorT pos, double * x, double * y, double * z, double * viewModel, double * projection, int * viewport);
int Vec_MatrixToAxisAngle( double* m, vec4_t * output );
void Vec_AxisAngleToQuaternion( vec4_t * var );
void Vec_QuatToEuler( vec4_t * q, vectorT * result );
void Vec_EulerToQuat( vectorT * euler, vec4_t * result );
void Vec_MulQuat( vec4_t * a, vec4_t * b );
void Vec_NomalizeQuat( vec4_t * q );
void Vec_QuatToMat( vec4_t * q, double * m );
void Vec_MulMat( double * ma, double * mb );
void Vec_MulVecMat( vectorT * vec, double * m );
void Vec_IdentityMat( double * m );

#endif
