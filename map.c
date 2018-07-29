#include "map.h"
#include "sys.h"
#include "util.h"
#include "vid.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

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
				double ** ptr = Sys_Malloc( count * sizeof(double*) );
				ptr[count-1] = Sys_Malloc( 4 * 4 * sizeof(double) );
				while( !LoadMatrix( file, ptr[count-1] ) ) {
					count++;
					ptr = Sys_Realloc( ptr, count * sizeof(double*) );
					ptr[count-1] = Sys_Malloc( 4 * 4 * sizeof(double) );
				}
				map.Tracks[map.numTracks-1]->cameraModelViewMatrices = ptr;
				map.Tracks[map.numTracks-1]->numFrames = count;
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

void Map_GetStartingPos( vectorT * target, uint index)
{
	*target = map.Tracks[0]->Vertices[0];
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
	minDistanceLength += Vec_LineToVecProjection( Vertices[minDistanceIndex-1], Vertices[minDistanceIndex], target) * VEC_DISTANCE( Vertices[minDistanceIndex-1], Vertices[minDistanceIndex] );
	
	// 3. Calculate frame number
	return MAX( 0, MIN( (int) map.Tracks[0]->numFrames, (int) ( (double) minDistanceLength * ( (double) map.Tracks[0]->numFrames / (double) map.Tracks[0]->Length ) ) ) );
}

void Map_Project( vectorT target, vectorT * result, uint frame){
	uint index = MIN( frame, map.Tracks[0]->numFrames );
	int viewport[4] = { 0, 0, BASEWIDTH, BASEHEIGHT };
	double x, y, z;
	Vec_Project( target, &x, &y, &z, map.Tracks[0]->cameraModelViewMatrices[index], map.Tracks[0]->cameraProjectionMatrix, &viewport);
	result->x = x;
	result->y = BASEHEIGHT-y;
	result->z = z;
}
