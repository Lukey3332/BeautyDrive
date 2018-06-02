
#ifndef UTIL_H
#define UTIL_H

typedef unsigned int uint;

#define SIZE_T uint

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define DYN_T(type, name) \
	SIZE_T dyn_num_##name = 0; \
	type * name;

#define DYN_PUSH( type, name, var ) \
	dyn_num_##name++; \
	name = Sys_Realloc( name, dyn_num_##name * sizeof ( type ) ); \
	name[dyn_num_##name-1] = var; 

#endif
