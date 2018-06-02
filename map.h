#include "util.h"
typedef struct { float x; float y; float z; } vectorT;

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

void LoadTrack ( void * fd, trackT * track );
int Map_Load ( char * path );
