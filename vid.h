#include "util.h"

#ifndef VID_H
#define VID_H

#define BASEWIDTH	800
#define BASEHEIGHT	480
#define BASEDEPTH	24

enum { RGB, ARGB, YUV };
enum { BLEND_ALPHA = 1, BLEND_KEY = 2, UPDATE_STATIC = 4 };

typedef void surface;

void Vid_Init ();
surface * Vid_CreateSurface (int format, int features, uint width, uint height);
surface * Vid_GetAndLockBuffer (int format, surface * ref);
void Vid_UnlockBuffer (int format, surface * ref);
void Vid_BlitBuffer (int format, surface * ref, uint x, uint y);
void Vid_BlitBufferScaled (int format, surface * ref, uint x, uint y, uint w, uint h);
void Vid_Blank ();
void Vid_Update ();
void Vid_Shutdown ();

#endif
