#include "util.h"
#include "vector.h"
#include "vid.h"

typedef struct {
	vectorT pos;
	vec4_t orientation;
} pointT;

typedef struct {
	uint width, height;
	uint center_x, center_y;
	surface ** images;
	uint num_images;
} renderdataT;

typedef struct {
	pointT point;
	vectorT velocity;
	renderdataT * renderdata;
} objectT;

typedef struct {
	uint numFrames;
	uint numVertices;
	vectorT * Vertices;
	double ** cameraModelViewMatrices;
	double * cameraProjectionMatrix;
	double Length;
} trackT;

typedef struct {
	uint numTracks; // a map can have multiple tracks for branches, scripted events, etc.
	trackT ** Tracks;
	uint numStarts; // also multiple starts for variation
	pointT * Starts;
	vectorT Source; // where is the top left corner of our map
	float Width, Weight;
} mapT;

int Map_Load ( char * path );
void Map_GetStartingPos( pointT * target, uint index );
uint Map_ToFrame( vectorT target );
void Map_Project( vectorT target, vectorT * result, uint frame );
int Map_CameraOrientation( vec4_t * result, uint frame);
surface * Map_LoadPNG( char * filename );
void Map_LoadRenderdata( renderdataT * renderdata, char * dirname );
