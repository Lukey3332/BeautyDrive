#include "util.h"
typedef enum { REWIND, SEEK, DIRECTION } seekT;

void Dec_Init ();
void Dec_LoadBackground ( const char * url );
void Dec_DrawBackground ();
int Dec_Seek ( seekT type, int pos );
uint Dec_Update ( uint frame );
void Dec_Shutdown ();
