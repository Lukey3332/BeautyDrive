#include "util.h"

void Vid_Init ();
void * Vid_CreateRGBSurface (uint width, uint height);
void * Vid_CreateYUVSurface (uint width, uint height);
void * Vid_GetAndLockRGBBuffer (void * ref);
void * Vid_GetAndLockYUVBuffer (void * ref);
void Vid_UnlockRGBBuffer (void * ref);
void Vid_UnlockYUVBuffer (void * ref);
void Vid_UpdateRGBBuffer (void * ref, uint x, uint y);
void Vid_UpdateYUVBuffer (void * ref, uint x, uint y);
void Vid_Blank ();
void Vid_Update ();
void Vid_Shutdown ();

