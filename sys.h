#include "util.h"
struct {
	int argc;
	char ** argv;
} sys_params;

void Sys_Printf (char *fmt, ...);
void Sys_Error (char *fmt, ...);
void Sys_Sleep (uint n);
uint Sys_Time ();
void Sys_Quit ();
void Sys_Init ();


