#include "util.h"

#define BASEWIDTH	640
#define BASEHEIGHT	480
#define BASEDEPTH	24

enum { STREAMING, STATIC };

void Vid_Init ();
void * Vid_CreateRGBSurface (uint width, uint height, int update);
void * Vid_CreateYUVSurface (uint width, uint height, int update);
void * Vid_GetAndLockRGBBuffer (void * ref);
void * Vid_GetAndLockYUVBuffer (void * ref);
void Vid_UnlockRGBBuffer (void * ref);
void Vid_UnlockYUVBuffer (void * ref);
void Vid_BlitRGBBuffer (void * ref, uint x, uint y);
void Vid_BlitYUVBuffer (void * ref, uint x, uint y);
void Vid_Blank ();
void Vid_Update ();
void Vid_Shutdown ();

