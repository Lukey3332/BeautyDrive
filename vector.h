typedef struct { float x; float y; float z; } vectorT;

#define VEC_DISTANCE( a, b ) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) )
#define VEC_DISTANCE_SQUARED( a, b ) ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z))

float Vec_LineToVecDistance( vectorT v, vectorT w, vectorT point);
float Vec_LineToVecProjection( vectorT v, vectorT w, vectorT point);
int Vec_Project( vectorT pos, double * x, double * y, double * z, double * viewModel, double * projection, int * viewport);
