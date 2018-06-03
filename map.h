#include "util.h"
#include "vector.h"

typedef struct {
	uint numVertices;
	vectorT * Vertices;
	double Length;
} trackT;

typedef struct {
	uint numTracks; // a map can have multiple tracks for branches, events, etc.
	trackT ** Tracks;
	uint numStarts; // also multiple starts for variation
	vectorT ** Starts;
	vectorT Source; // where is the top left corner of our map
	float Width, Weight;
} mapT;

int Map_Load ( char * path );
void Map_GetStartingPos( vectorT * target, uint index);
uint Map_ToFrame( vectorT target, uint numVideoFrames);
