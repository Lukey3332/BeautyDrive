#include "util.h"
typedef enum { RGB, YUV } Vid_SurfaceType;
typedef uint Vid_SurfaceRef;

void Vid_Init ();
Vid_SurfaceRef Vid_CreateSurface (uint width, uint height, Vid_SurfaceType type);
void * Vid_GetAndLockBuffer (Vid_SurfaceRef ref);
void Vid_UnlockBuffer (Vid_SurfaceRef ref);
void Vid_UpdateBuffer(Vid_SurfaceRef ref, uint x, uint y);
void Vid_Update ();
void Vid_Shutdown ();
