#include "SDL.h"
#include "sys.h"
#include "vid.h"
#include "dec.h"
#include "game.h"

Sys_ArgsT Sys_Args;

void * Sys_OpenFile (char * path, openT mode)
{
	FILE * tmp;
	switch(mode) {
		case READ:
			tmp = fopen(path, "r");
		break;
		
		case WRITE:
			tmp = fopen(path, "w");
		break;
	}
	
	return tmp;
}

void Sys_CloseFile (void * handle)
{
	fclose(handle);
}

//uint Sys_FileRead ( void * handle, void * buf, uint count)
//{
	
//}

void * Sys_FileReadLine ( void * handle, void * buf, uint count)
{
	return fgets( buf, count, handle);
}

void * Sys_Malloc ( SIZE_T size)
{
	return malloc( size );
}

void * Sys_Realloc (void * ptr, SIZE_T size)
{
	return realloc( ptr, size);
}

void Sys_Free (void * ptr)
{
	free(ptr);
}

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
	Sys_Args.argc = argc;
	Sys_Args.argv = argv;
	Sys_Init();
	Vid_Init();
	Dec_Init();
	Game_Init();
	
	while (1) {
		Game_Frame();
	}
}
