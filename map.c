#include "map.h"
#include "sys.h"
#include "util.h"
#include "vid.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <png.h>

#define BUFSIZE 256
char buf[BUFSIZE];

DYN_T( vectorT, tmpVecs )
mapT map;

static void LoadTrack ( void * fd, trackT * track )
{
	memset( track, 0x00, sizeof(trackT));
	int firstline = 0;
	while( Sys_FileReadLine( fd, buf, BUFSIZE) ){
		if( strncmp( buf, "v ", 2 ) == 0 ){ // Save Vertices
			vectorT vertex;
			sscanf( buf, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			DYN_PUSH( vectorT, tmpVecs, vertex)
			
		}else if( strncmp( buf, "l ", 2 ) == 0 ){
			int indexFrom, indexTo;
			vectorT from, to;
			sscanf( buf, "l %d %d\n", &indexFrom, &indexTo );
			from = tmpVecs[indexFrom-1];
			to = tmpVecs[indexTo-1];
			if(!firstline) { // HACKY
				track->numVertices++;
				track->Vertices = Sys_Realloc( track->Vertices, track->numVertices * sizeof( vectorT ) );
				track->Vertices[track->numVertices-1] = from;
				firstline = 1;
			}
			track->numVertices++;
			track->Vertices = Sys_Realloc( track->Vertices, track->numVertices * sizeof( vectorT ));
			track->Vertices[track->numVertices-1] = to;
			
			//Measure distance and add it to the Length
			track->Length += VEC_DISTANCE( from, to);
		} else break;
	}
}

static int LoadMatrix( void * fd, double * matrix )
{
	memset( matrix, 0x00, 4 * 4 * sizeof(double));
	for(uint n = 0; n < 4; n++){
		if( !Sys_FileReadLine( fd, buf, BUFSIZE) ) return 1;
		if( strncmp( buf, "m ", 2 ) != 0 ) return 1;
		// We need to convert from columns to rows here...
		sscanf( buf, "m %lf %lf %lf %lf\n", &matrix[ n + (0 * 4)], &matrix[ n + (1 * 4)], &matrix[ n + (2 * 4)], &matrix[ n + (3 * 4)] );
	}
	return 0;
}

int Map_Load ( char * path )
{
	void * file;
	file = Sys_OpenFile( path, READ);
	
	if(!file) return 1;
	map.numStarts = 0;
	
	while( Sys_FileReadLine( file, buf, BUFSIZE) ){
		switch(buf[0]) {
			case 'o':
				if ( strncmp( &buf[2], "track_", 6 ) == 0 ){
					map.numTracks++;
					map.Tracks = Sys_Realloc( map.Tracks, map.numTracks * sizeof( trackT * ));
					map.Tracks[map.numTracks-1] = Sys_Malloc( sizeof( trackT ) );
					LoadTrack( file, map.Tracks[map.numTracks-1] );
				} else if ( strncmp( &buf[2], "map_", 4 ) == 0 ){
					
				}
			break;
			
			case 'c':
				if(!map.Tracks) return 1;
				// First load the static Projection Matrix
				map.Tracks[map.numTracks-1]->cameraProjectionMatrix = Sys_Malloc( 4 * 4 * sizeof(double) );
				LoadMatrix( file, map.Tracks[map.numTracks-1]->cameraProjectionMatrix );

				// Then load the ModelView Matrix Frame by frame
				uint count = 1;
				double ** cptr = Sys_Malloc( count * sizeof(double*) );
				cptr[count-1] = Sys_Malloc( 4 * 4 * sizeof(double) );
				while( !LoadMatrix( file, cptr[count-1] ) ) {
					count++;
					cptr = Sys_Realloc( cptr, count * sizeof(double*) );
					cptr[count-1] = Sys_Malloc( 4 * 4 * sizeof(double) );
				}
				map.Tracks[map.numTracks-1]->cameraModelViewMatrices = cptr;
				map.Tracks[map.numTracks-1]->numFrames = count-1;
			break;
			
			case 'a':
				if(map.numStarts == 0) {
					map.numStarts = 1;
					map.Starts = Sys_Malloc( sizeof(pointT) );
				} else {
					map.numStarts++;
					map.Starts = Sys_Realloc( map.Starts, map.numStarts * sizeof(pointT) );
				}
				pointT * aptr = &map.Starts[map.numStarts-1];
				sscanf( buf, "a %f %f %f %f %f %f %f", &aptr->pos.x, &aptr->pos.y, &aptr->pos.z, &aptr->orientation.x, &aptr->orientation.y, &aptr->orientation.z, &aptr->orientation.w );
			break;
		}
	}
	
	Sys_CloseFile( file );
	
	// Safety Checks
	for( uint n = 0; n < map.numTracks; n++ ){
		if( map.Tracks[n]->Vertices == NULL || 
			map.Tracks[n]->cameraModelViewMatrices == NULL ||
			map.Tracks[n]->cameraProjectionMatrix == NULL ){
				return 1; // Something went wrong...
		}
	}
	
	return 0;
}

void Map_GetStartingPos( pointT * target, uint index)
{
	*target = map.Starts[index];
}

uint Map_ToFrame( vectorT target)
{
	// 1. Search closest line to our Target and the line position on the track(set of lines)
	vectorT * Vertices = map.Tracks[0]->Vertices;
	uint numVertices = map.Tracks[0]->numVertices;
	float minDistance;
	uint minDistanceIndex;
	double minDistanceLength = 0.0;
	double length = 0.0;
	
	minDistance = Vec_LineToVecDistance( Vertices[0], Vertices[1], target);
	minDistanceIndex = 1;
	for (uint n = 1; n<numVertices; n++ ){
		length += VEC_DISTANCE( Vertices[n-1], Vertices[n]);
		float Distance = Vec_LineToVecDistance( Vertices[n-1], Vertices[n], target);
		if(Distance < minDistance){
			minDistanceLength = length;
			minDistance = Distance;
			minDistanceIndex = n;
		}
	}
	
	// 2. Add the distance on the line to it
	if(minDistanceIndex+1 < numVertices) minDistanceLength += Vec_LineToVecProjection( Vertices[minDistanceIndex], Vertices[minDistanceIndex+1], target) * VEC_DISTANCE( Vertices[minDistanceIndex], Vertices[minDistanceIndex+1] );
	
	// 3. Calculate frame number
	return MAX( 0, MIN( (int) map.Tracks[0]->numFrames, (int) ( (double) minDistanceLength * ( (double) map.Tracks[0]->numFrames / (double) map.Tracks[0]->Length ) ) ) );
}

void Map_Project( vectorT target, vectorT * result, uint frame)
{
	uint index = MIN( frame-1, map.Tracks[0]->numFrames-1 );
	int viewport[4] = { 0, 0, BASEWIDTH, BASEHEIGHT };
	double x, y, z;
	Vec_Project( target, &x, &y, &z, map.Tracks[0]->cameraModelViewMatrices[index], map.Tracks[0]->cameraProjectionMatrix, (int *)&viewport);
	result->x = x;
	result->y = BASEHEIGHT-y;
	result->z = z;
}

int Map_CameraOrientation( vec4_t * result, uint frame)
{
	uint index = MIN( frame-1, map.Tracks[0]->numFrames-1 );
	int ret = Vec_MatrixToAxisAngle( map.Tracks[0]->cameraModelViewMatrices[index], result );
	Vec_AxisAngleToQuaternion( result );
	return ret;
}

surface * Map_LoadPNG( char * filename )
{
	int width, height;
	png_byte color_type, bit_depth;
	png_bytep *row_pointers;
	
	void * fd = Sys_OpenFile( filename, READ);
	
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) abort();
	
	png_infop info = png_create_info_struct(png);
	if(!info) abort();
	
	if(setjmp(png_jmpbuf(png))) abort();
	
	png_init_io(png, fd);
	
	png_read_info(png, info);
	
	width = png_get_image_width(png, info);
	height = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth = png_get_bit_depth(png, info);
	
	if(color_type != PNG_COLOR_TYPE_RGBA ) abort();
	
	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	
	surface * surf = Vid_CreateSurface( ARGB, BLEND_ALPHA, width, height );
	surface * buf = Vid_GetAndLockBuffer( ARGB, surf );
	
	png_read_update_info(png, info);
	
	row_pointers = (png_bytep*)Sys_Malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++) {
		row_pointers[y] = buf+(width*4*y);
	}
	
	png_read_image(png, row_pointers);
	
	Vid_UnlockBuffer( ARGB, surf );
	
	Sys_CloseFile( fd );
	
	return surf;
}

void Map_LoadRenderdata( renderdataT * renderdata, char * dirname )
{
	renderdata->width = 320;
	renderdata->height = 240;
	renderdata->center_x = 160;
	renderdata->center_y = 192;
	renderdata->num_images = 180;
	renderdata->images = malloc( 180 * sizeof(surface *) );

	char buf[128];
	for( uint n = 0; n<180; n++ ){
		sprintf( buf, "%s/%04u.png", dirname, n);
		renderdata->images[n] = Map_LoadPNG( buf );
	}
}
