#include "map.h"
#include "sys.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define BUFSIZE 256

DYN_T( vectorT, tmpVecs )
mapT map;

static void LoadTrack ( void * fd, trackT * track )
{
	char buf[BUFSIZE];
	memset( track, 0x00, sizeof(trackT));
	int firstline = 0;
	while( Sys_FileReadLine( fd, buf, BUFSIZE) ){
		if( strncmp( buf, "v ", 2 ) == 0 ){ // Save Vertices
			vectorT vertex;
			sscanf( buf, "v %f %f %f\n", &vertex.x, &vertex.z, &vertex.y );
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
		}
	}
	Sys_Printf("Length: %f\n", track->Length);
}

int Map_Load ( char * path )
{
	void * file;
	char buf[BUFSIZE];
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
				}else if ( strncmp( &buf[2], "map_", 4 ) == 0 ){
					
				}
		}
	}
	return 0;
}

void Map_GetStartingPos( vectorT * target, uint index)
{
	*target = map.Tracks[0]->Vertices[0];
}

uint Map_ToFrame( vectorT target, uint numVideoFrames)
{
	// 1. Search closest line to our Target and the line position on the track(set of lines)
	vectorT * Vertices = map.Tracks[0]->Vertices;
	uint numVertices = map.Tracks[0]->numVertices;
	float minDistance;
	uint minDistanceIndex;
	double minDistanceLength = 0.0;
	double length = 0.0;
	
	minDistance = line_to_vec_distance( Vertices[0], Vertices[1], target);
	minDistanceIndex = 1;
	for (uint n = 1; n<numVertices; n++ ){
		length += VEC_DISTANCE( Vertices[n-1], Vertices[n]);
		float Distance = line_to_vec_distance( Vertices[n-1], Vertices[n], target);
		if(Distance < minDistance){
			minDistanceLength = length;
			minDistance = Distance;
			minDistanceIndex = n;
		}
	}
	
	// 2. Add the distance on the line to it
	minDistanceLength += line_to_vec_projection( Vertices[minDistanceIndex-1], Vertices[minDistanceIndex], target) * VEC_DISTANCE( Vertices[minDistanceIndex-1], Vertices[minDistanceIndex] );
	
	// 3. Calculate frame number
	return minDistanceLength * ( numVideoFrames / map.Tracks[0]->Length );
}
