#include "SDL.h"
#include "sys.h"
#include "vid.h"
#include "dec.h"
#include "game.h"

void Sys_Printf (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];
	
	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);
	fprintf(stderr, "%s\n", text);
}

void Sys_Error (char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];
	
	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);
	fprintf(stderr, "Error: %s\n", text);
	
	exit(1);
}

void Sys_Sleep (uint n)
{
	SDL_Delay(n);
}

uint Sys_Time()
{
	return SDL_GetTicks();
}

void Sys_Init ()
{
	SDL_Init(0);
}

void Sys_Quit ()
{
	Dec_Shutdown();
	Vid_Shutdown();
	SDL_Quit();
	exit(0);
}

int main (int argc, char *argv[])
{
	sys_params.argc = argc;
	sys_params.argv = argv;
	Sys_Init();
	Vid_Init();
	Dec_Init();
	Game_Init();
	
	while (1) {
		Game_Frame();
	}
}
