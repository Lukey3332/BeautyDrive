#include "map.h"
#include "sys.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define BUFSIZE 256

DYN_T( vectorT, tmpVecs )
mapT map;

void LoadTrack ( void * fd, trackT * track )
{
	char buf[BUFSIZE];
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
			
			double xSqr = (from.x - to.x) * (from.x - to.x);
			double ySqr = (from.y - to.y) * (from.y - to.y);
			double zSqr = (from.z - to.z) * (from.z - to.z);
			track->Length += sqrt(xSqr + ySqr + zSqr);
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
