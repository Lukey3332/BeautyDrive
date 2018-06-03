typedef struct { float x; float y; float z; } vectorT;

#define VEC_DISTANCE( a, b ) sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) )
#define VEC_DISTANCE_SQUARED( a, b ) ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z))

float line_to_vec_distance( vectorT v, vectorT w, vectorT point);
float line_to_vec_projection( vectorT v, vectorT w, vectorT point);
