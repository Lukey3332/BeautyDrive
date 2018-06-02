#include "util.h"
struct { int argc; char ** argv;} Sys_Args;
typedef enum { READ, WRITE } openT;

void * Sys_OpenFile ( char * path, openT mode);
void Sys_CloseFile ( void * handle);
uint Sys_FileRead ( void * handle, void * buf, uint count);
void * Sys_FileReadLine ( void * handle, void * buf, uint count);
void * Sys_FileReadLine ( void * handle, void * buf, uint count);
void * Sys_Malloc ( SIZE_T size);
void * Sys_Realloc ( void * ptr, SIZE_T size);
void Sys_Free (void * ptr);
void Sys_Printf (char *fmt, ...);
void Sys_Error (char *fmt, ...);
void Sys_Sleep (uint n);
uint Sys_Time ();
void Sys_Quit ();
void Sys_Init ();


