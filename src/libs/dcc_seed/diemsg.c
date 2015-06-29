#include <dcc_std.h>
#include <dcc_seed.h>

_SUB BOOL diemsg(char *file, int line, char *date)
{

	fprintf(stderr,"Routine aborted %s (%d) %s\n",file,line,date);

	return(FALSE);

}
