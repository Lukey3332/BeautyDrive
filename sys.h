struct {
	int argc;
	char ** argv;
} sys_params;

void Sys_Quit ();
void Sys_Init ();
void Sys_Error (char *fmt, ...);
void Sys_Printf (char *fmt, ...);
