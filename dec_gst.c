#include <gst/gst.h>
#include "sys.h"

void Dec_Init ()
{
	gst_init (&sys_params.argc, &sys_params.argv);
}

void Dec_Shutdown ()
{
	
}
